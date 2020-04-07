#include "async.h"

using namespace Nan;

uv_async_t *async = NULL;
uv_mutex_t lock;

static void default_loop_entry(uv_async_t* handle) {
	call_info_t *info = (call_info_t*) handle->data;

	info->fn(info->args);
	// Unlock run_on_default_loop
	uv_sem_post(&info->sem);
}

void init_async() {
	async = new uv_async_t;
	uv_async_init(uv_default_loop(), async, default_loop_entry);
	uv_mutex_init(&lock);
}

void close_async() {
	// Don't call uv_mutex_destroy(&lock); here bacause it may be locked by
	// run_on_default_loop
	uv_close((uv_handle_t*)async, [](uv_handle_t* handle) {
		delete handle;
    });
}

void run_on_default_loop(void (*fn)(void *), void *args) {
	// This function needs to be mutex'd because uv_async_send can coalesce
	// calls if called multiple times. This could potentially be avoided with a
	// heap allocated queue that stores requests for the main thread but the
	// benefit would be small.
	uv_mutex_lock(&lock);

	call_info_t info;
	info.fn = fn;
	info.args = args;
	uv_sem_init(&info.sem, 0);

	async->data = &info;
	uv_async_send(async);

	uv_sem_wait(&info.sem);
	uv_mutex_unlock(&lock);
}

static NAN_METHOD(callback_wrapper) {
	if ((async == NULL) || (uv_is_closing((uv_handle_t*)async) != 0)) {
		return;
	}
	auto context = info.GetIsolate()->GetCurrentContext();
	auto data = info.Data()->ToObject(context).ToLocalChecked();

	void (*fn)(NAN_METHOD_ARGS_TYPE, void *) = Get(data, (uint32_t)0).ToLocalChecked().As<v8::External>()->Value();
	auto args = data->Get(1).As<v8::External>()->Value();

	if (fn && args) {
		// Ensure callback is called only once
		Delete(data, 0);
		Delete(data, 1);

		fn(info, args);
	}
}

v8::Local<v8::Function> make_callback(void (*fn)(NAN_METHOD_ARGS_TYPE, void *), void *args) {
	auto context = GetCurrentContext();
    auto data = New<v8::Object>();
	data->Set(0, New<v8::External>(fn));
	data->Set(1, New<v8::External>(args));

	return New<v8::FunctionTemplate>(callback_wrapper, data)->GetFunction(context).ToLocalChecked();
}
