#ifndef _USER_PTHREAD_H_
#define _USER_PTHREAD_H_

#include "lib.h"

#define PTHREAD_THREADS_MAX 64
#define PTHREAD_KEYS_MAX 64
#define PTHREAD_DESTRUCTOR_ITERATIONS 1
#define PTHREAD_STACK_MIN (BY2PG * 3)

#define _PTHREAD_ATFORK_MAX 16

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_CANCELED ((void *)-1)

#define EINVAL 22
#define EAGAIN 112
#define ENOMEM 132

typedef u_int pthread_t;
typedef u_int pthread_key_t;

typedef struct {
	int detachstate;
} pthread_attr_t;

extern void (*_pthread_prepare_hooks[])();
extern void (*_pthread_parent_hooks[])();
extern void (*_pthread_child_hooks[])();
extern size_t _pthread_atfork_count;

void _pthread_init();
void _pthread_proc_exit();

pthread_t pthread_self();
int pthread_equal(pthread_t t1, pthread_t t2);

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);

int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel();

int pthread_create(
	pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void *), void *arg);
void pthread_exit(void *value_ptr);
int pthread_cancel(pthread_t thread);
int pthread_detach(pthread_t thread);
int pthread_join(pthread_t thread, void **value_ptr);

int pthread_atfork(void (*prepare)(), void (*parent)(), void (*child)());

#endif
