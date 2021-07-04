#include "lib.h"
#include "pthread.h"

pthread_key_t key;

void cleanup(void *arg) {
	writef("cleanup handler called: %d\n", (int)arg);
}

void *thread_fn(void *arg) {
	writef("worker: stack: %d\n", *(int *)arg);
	++*(int *)arg;
	writef("worker: tls: %d\n", (int)pthread_getspecific(key));
	user_assert(!pthread_setspecific(key, (void *)321));
	writef("worker: tls: %d\n", (int)pthread_getspecific(key));

	pthread_cleanup_push(&cleanup, (void *)1);
	pthread_cleanup_push(&cleanup, (void *)2);
	pthread_cleanup_pop(1);
	writef("worker: waiting for cancellation\n");
	while (1) {
		pthread_testcancel();
	}
	pthread_cleanup_pop(0);
	return NULL;
}

void key_dtor(void *val) {
	pthread_t self = pthread_self();
	writef("thread %u: key destructor called: %d\n", self, (int)val);
}

void umain() {
	int x = 42;
	void *stat;

	user_assert(!pthread_key_create(&key, &key_dtor));
	user_assert(!pthread_setspecific(key, (void *)123));
	writef("main: tls: %d\n", (int)pthread_getspecific(key));

	writef("main: starting worker\n");
	pthread_t worker;
	user_assert(!pthread_create(&worker, NULL, &thread_fn, &x));
	writef("main: cancelling worker\n");
	user_assert(!pthread_cancel(worker));
	user_assert(!pthread_join(worker, &stat));
	writef("main: worker exited: %d\n", (int)stat);

	writef("main: stack: %d\n", x);
	writef("main: tls: %d\n", (int)pthread_getspecific(key));

	user_assert(!pthread_detach(pthread_self()));
	pthread_exit(NULL);
}
