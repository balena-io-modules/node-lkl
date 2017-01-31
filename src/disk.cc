extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include <nan.h>
#include "async.h"

#define SECTOR_SIZE 512

using namespace Nan;

typedef struct lkl_disk lkl_disk_t;
typedef struct lkl_blk_req lkl_blk_req_t;

/*
 * Defines the Javascript counterparts of lkl_dev_blk_ops
 */
struct js_ops_t {
	Callback* request;
	Callback* get_capacity;
};

/*
 * state structs are used to pass around variables between functions running in
 * different threads and times. They are created in the stack of the first
 * function and a pointer is passed around while the first function waits for
 * all the others to finish.
 */
struct get_capacity_state_t {
	int ret;
	lkl_disk_t disk;
	unsigned long long *res;
	uv_sem_t js_sem;
};

struct request_state_t {
	int ret;
	lkl_disk_t disk;
	lkl_blk_req_t *req;
	uv_sem_t js_sem;
};

static void get_capacity_done(NAN_METHOD_ARGS_TYPE info, get_capacity_state_t* s) {
	if (info[0]->IsNull()) {
		s->ret = 0;
		*(s->res) = info[1]->IntegerValue();
	} else {
		s->ret = info[0]->IntegerValue();
		*(s->res) = 0;
	}

	// Unblocks the original lkl thread
	uv_sem_post(&s->js_sem);
}

static void get_capacity(get_capacity_state_t *s) {
	HandleScope scope;
	auto js_ops = static_cast<js_ops_t*>(s->disk.handle);

	v8::Local<v8::Value> argv[] = {
		make_callback(get_capacity_done, static_cast<void *>(s))
	};

	js_ops->get_capacity->Call(1, argv);
}

static void request_done(NAN_METHOD_ARGS_TYPE info, request_state_t* s) {
	if (info[0]->IsNull()) {
		s->ret = 0;
	} else {
		s->ret = info[0]->IntegerValue();
	}

	// Unblocks the original lkl thread
	uv_sem_post(&s->js_sem);
}

static void noop(char *data, void *hint) {}

static void request(request_state_t *s) {
	HandleScope scope;
	int i;

	// Convert iovecs to Buffer objects pointing to the same memory
	auto iovecs = New<v8::Array>();
	for (i = 0; i < s->req->count; i++) {
		// We pass `noop` as the free callback because memory is managed by lkl
		// and we don't want V8 to run free() on it
		Set(iovecs, i, NewBuffer((char*) s->req->buf[i].iov_base,
								 s->req->buf[i].iov_len, noop, NULL).ToLocalChecked());
	}

	auto js_ops = static_cast<js_ops_t*>(s->disk.handle);

	v8::Local<v8::Value> argv[] = {
		New<v8::Number>(s->req->type),
		New<v8::Number>(s->req->sector * SECTOR_SIZE),
		iovecs,
		make_callback(request_done, static_cast<void *>(s)),
	};

	js_ops->request->Call(4, argv);
}

static int js_get_capacity_entry(lkl_disk_t disk, unsigned long long *res) {
	get_capacity_state_t s;
	s.disk = disk;
	s.res = res;
	uv_sem_init(&s.js_sem, 0);

	// We are in a lkl thread, we can't call JS functions directly
	run_on_default_loop(get_capacity, &s);

	uv_sem_wait(&s.js_sem);
	uv_sem_destroy(&s.js_sem);
	return s.ret;
}

static int js_request_entry(lkl_disk_t disk, lkl_blk_req_t *req) {
	request_state_t s;
	s.disk = disk;
	s.req = req;
	uv_sem_init(&s.js_sem, 0);

	// We are in a lkl thread, we can't call JS functions directly
	run_on_default_loop(request, &s);

	uv_sem_wait(&s.js_sem);
	uv_sem_destroy(&s.js_sem);
	return s.ret;
}

struct lkl_dev_blk_ops lkl_js_disk_ops = {
	.get_capacity = js_get_capacity_entry,
	.request = js_request_entry,
};

class MountWorker : public AsyncWorker {
	public:
		MountWorker(NAN_METHOD_ARGS_TYPE info, Callback *callback)
		: AsyncWorker(callback) {
			readonly = info[0]->BooleanValue();

			Utf8String fs_type_(info[1]);
			strncpy(fs_type, *fs_type_, sizeof(fs_type) - 1);
			fs_type[sizeof(fs_type) - 1] = '\0';

			part = info[2]->Uint32Value();

			// FIXME: delete this object when the disk is unmounted
			auto js_ops = new js_ops_t;
			js_ops->request = new Callback(info[3].As<v8::Function>());
			js_ops->get_capacity = new Callback(info[4].As<v8::Function>());

			disk.ops = &lkl_js_disk_ops;
			disk.handle = js_ops;
		}

		~MountWorker() {}

		void Execute () {
			disk_id = lkl_disk_add(&disk);
			ret = lkl_mount_dev(disk_id, part, fs_type, readonly ? LKL_MS_RDONLY :
								0, NULL, mountpoint, sizeof(mountpoint));
		}

		void HandleOKCallback () {
			HandleScope scope;

			if (ret < 0) {
				v8::Local<v8::Value> argv[] = {
					New<v8::Number>(-ret)
				};
				callback->Call(1, argv);
			} else {
				v8::Local<v8::Object> ret2 = New<v8::Object>();
				Set(ret2, New("mountpoint").ToLocalChecked(), New(mountpoint).ToLocalChecked());
				Set(ret2, New("diskId").ToLocalChecked(), New(disk_id));
				v8::Local<v8::Value> argv[] = { Null(), ret2 };
				callback->Call(2, argv);
			}
		}

	private:
		long ret;
		bool readonly;
		unsigned int part;
		char fs_type[10];
		char mountpoint[32];
		unsigned int disk_id;
		lkl_disk_t disk;
};

NAN_METHOD(mount) {
	if (info.Length() != 6) {
		ThrowTypeError("Wrong number of arguments");
		return;
	}

	if (info[5]->IsFunction()) {
		Callback *callback = new Callback(info[5].As<v8::Function>());
		AsyncQueueWorker(new MountWorker(info, callback));
	}
}

NAN_METHOD(umount) {
	unsigned int disk_id;
	int ret;

	if (info.Length() != 1) {
		ThrowTypeError("Wrong number of arguments");
		return;
	}

	disk_id = info[0]->IntegerValue();

	lkl_sys_sync();
	ret = lkl_umount_dev(disk_id, 0, 0, 1000);
	if (ret < 0) {
		ThrowError(ErrnoException(-ret));
	} else {
		info.GetReturnValue().Set(New(ret));
	}
}
