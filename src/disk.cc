extern "C" {
	#include <lkl.h>
	#include <lkl_host.h>
}

#include <unordered_map>
#include <nan.h>
#include "async.h"
#include "worker.h"

#define SECTOR_SIZE 512

#define CHECK_ARGS(length) \
	if (info.Length() != length) { \
		ThrowTypeError("Wrong number of arguments"); \
		return; \
	} \
	if (!info[length - 1]->IsFunction()) { \
		ThrowTypeError("A callback is required"); \
		return; \
	}

using namespace Nan;

typedef struct lkl_disk lkl_disk_t;
typedef struct lkl_blk_req lkl_blk_req_t;

std::unordered_map<unsigned int, lkl_disk_t> disks;

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
	auto context = info.GetIsolate()->GetCurrentContext();
	if (info[0]->IsNull()) {
		s->ret = 0;
		*(s->res) = info[1]->IntegerValue(context).ToChecked();
	} else {
		s->ret = info[0]->IntegerValue(context).ToChecked();
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
		auto context = info.GetIsolate()->GetCurrentContext();
		s->ret = info[0]->IntegerValue(context).ToChecked();
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

struct disk_add_state_t {
	int disk_id;
	Callback *callback;
};

static void disk_add_callback(disk_add_state_t *s) {
	HandleScope scope;
	if (s->disk_id < 0) {
		v8::Local<v8::Value> argv[] = {ErrnoException(-s->disk_id)};
		s->callback->Call(1, argv);
	} else {
		v8::Local<v8::Value> argv[] = { Null(), New(s->disk_id) };
		s->callback->Call(2, argv);
	}
}

struct disk_add_args_t {
	lkl_disk_t disk;
	Callback *callback;
};

void do_disk_add(disk_add_args_t *args) {
	int disk_id = lkl_disk_add(&(args->disk));
	disk_add_state_t s = {disk_id, args->callback};
	disks[disk_id] = args->disk;
	run_on_default_loop(disk_add_callback, &s);
	delete args->callback;
	delete args;
}

NAN_METHOD(disk_add) {
	CHECK_ARGS(3);
	auto js_ops = new js_ops_t;
	js_ops->request = new Callback(info[0].As<v8::Function>());
	js_ops->get_capacity = new Callback(info[1].As<v8::Function>());
	Callback *callback = new Callback(info[2].As<v8::Function>());
	lkl_disk_t disk;
	disk.ops = &lkl_js_disk_ops;
	disk.handle = js_ops;
	disk_add_args_t *args = new disk_add_args_t;
	args->disk = disk;
	args->callback = callback;
	run_on_worker_loop(do_disk_add, args);
}

struct disk_remove_state_t {
	int ret;
	Callback *callback;
};

static void disk_remove_callback(disk_remove_state_t *s) {
	HandleScope scope;
	v8::Local<v8::Value> argv[1];
	if (s->ret < 0) {
		argv[0] = ErrnoException(-s->ret);
	} else {
		argv[0] = Null();
	}
	s->callback->Call(1, argv);
}

struct disk_remove_args_t {
	unsigned int disk_id;
	Callback *callback;
};

void do_disk_remove(disk_remove_args_t *args) {
	lkl_disk_t disk = disks[args->disk_id];
	int ret = lkl_disk_remove(disk);
	disks.erase(args->disk_id);
	disk_remove_state_t s = {ret, args->callback};
	run_on_default_loop(disk_remove_callback, &s);
	delete args->callback;
	delete args;
}

NAN_METHOD(disk_remove) {
	CHECK_ARGS(2);
	auto context = info.GetIsolate()->GetCurrentContext();
	unsigned int disk_id = info[0]->Uint32Value(context).ToChecked();
	Callback *callback = new Callback(info[1].As<v8::Function>());
	disk_remove_args_t *args = new disk_remove_args_t;
	args->disk_id = disk_id;
	args->callback = callback;
	run_on_worker_loop(do_disk_remove, args);
}

struct mount_state_t {
	long ret;
	char* mountpoint;
	Callback *callback;
};

static void mount_callback(mount_state_t *s) {
	HandleScope scope;
	if (s->ret < 0) {
		v8::Local<v8::Value> argv[] = {ErrnoException(-s->ret)};
		s->callback->Call(1, argv);
	} else {
		v8::Local<v8::Value> argv[] = {
			Null(),
			New(s->mountpoint).ToLocalChecked()
		};
		s->callback->Call(2, argv);
	}
}

struct mount_args_t {
	unsigned int disk_id;
	bool readonly;
	const char* fs_type;
	unsigned int part;
	Callback *callback;
};

void do_mount(mount_args_t* args) {
	char mountpoint[32];
	long ret = lkl_mount_dev(
		args->disk_id,
		args->part,
		args->fs_type,
		args->readonly ? LKL_MS_RDONLY : 0, NULL,
		mountpoint,
		sizeof(mountpoint)
	);
	mount_state_t s = {ret, mountpoint, args->callback};
	run_on_default_loop(mount_callback, &s);
	delete args->fs_type;
	delete args->callback;
	delete args;
}

NAN_METHOD(mount) {
	CHECK_ARGS(5);
	auto context = info.GetIsolate()->GetCurrentContext();
	unsigned int disk_id = info[0]->Uint32Value(context).ToChecked();
	bool readonly = info[1]->BooleanValue(context).ToChecked();

	char *fs_type = new char[10];
	Utf8String fs_type_(info[2]);
	strncpy(fs_type, *fs_type_, sizeof(fs_type) - 1);
	fs_type[sizeof(fs_type) - 1] = '\0';

	unsigned int part = info[3]->Uint32Value(context).ToChecked();

	Callback *callback = new Callback(info[4].As<v8::Function>());

	mount_args_t* args = new mount_args_t;
	args->disk_id = disk_id;
	args->readonly = readonly;
	args->fs_type = fs_type;
	args->part = part;
	args->callback = callback;

	run_on_worker_loop(do_mount, args);
}

struct umount_state_t {
	int ret;
	Callback *callback;
};

static void umount_callback(umount_state_t *s) {
	HandleScope scope;
	if (s->ret < 0) {
		v8::Local<v8::Value> argv[] = {ErrnoException(-s->ret)};
		s->callback->Call(1, argv);
	} else {
		v8::Local<v8::Value> argv[] = {Null(), New<v8::Number>(s->ret)};
		s->callback->Call(2, argv);
	}
}

struct umount_args_t {
	unsigned int disk_id;
	unsigned int partition;
	Callback *callback;
};

void do_umount(umount_args_t* args) {
	lkl_sys_sync();
	int ret = lkl_umount_dev(args->disk_id, args->partition, 0, 1000);
	umount_state_t s = {ret, args->callback};
	run_on_default_loop(umount_callback, &s);
	delete args->callback;
	delete args;
}

NAN_METHOD(umount) {
	CHECK_ARGS(3);
	auto context = info.GetIsolate()->GetCurrentContext();
	unsigned int disk_id = info[0]->Uint32Value(context).ToChecked();
	unsigned int partition = info[1]->Uint32Value(context).ToChecked();
	Callback *callback = new Callback(info[2].As<v8::Function>());
	umount_args_t* args = new umount_args_t;
	args->disk_id = disk_id;
	args->partition = partition;
	args->callback = callback;
	run_on_worker_loop(do_umount, args);
}
