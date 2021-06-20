#include "lib.h"
#include "pthread.h"
#include "semaphore.h"
#include <queue.h>

#define THREAD_CANCEL_PENDING 1
#define THREAD_DETACHED 2
#define THREAD_ZOMBIE 4

int _pthread_disabled __attribute__((weak));

typedef struct tls_entry {
	LIST_ENTRY(tls_entry) link;
	void (*dtor)(void *);
} tls_entry_t;

LIST_HEAD(tls_entry_head, tls_entry);
static struct tls_entry_head tls_entry_free_list;
static tls_entry_t tls_entries[PTHREAD_KEYS_MAX];

typedef struct thread {
	LIST_ENTRY(thread) link;
	u_int env_id;
	void *exit_status;
	sem_t sem;
	int flags;
	int cancel_state;
	int cancel_type;
	void *tls[PTHREAD_KEYS_MAX];
} thread_t;

LIST_HEAD(thread_head, thread);
static struct thread_head thread_free_list;
static thread_t threads[PTHREAD_THREADS_MAX];

void (*_pthread_prepare_hooks[_PTHREAD_ATFORK_MAX])();
void (*_pthread_parent_hooks[_PTHREAD_ATFORK_MAX])();
void (*_pthread_child_hooks[_PTHREAD_ATFORK_MAX])();
size_t _pthread_atfork_count;

static sem_t global_mutex;

static void pthread_reinit() {
	int i;

	LIST_INIT(&thread_free_list);
	for (i = PTHREAD_THREADS_MAX - 1; i >= 1; i--) {
		threads[i].env_id = 0;
		LIST_INSERT_HEAD(&thread_free_list, &threads[i], link);
	}

	threads[0].env_id = syscall_getenvid();

	if (sem_init(&threads[0].sem, 0, 0) ||
		sem_init(&global_mutex, 0, 1)) {
		user_panic("pthread_reinit: could not create semaphores");
	}
}

void _pthread_init() {
	int i;

	if (_pthread_disabled) {
		return;
	}

	LIST_INIT(&tls_entry_free_list);
	for (i = PTHREAD_KEYS_MAX - 1; i >= 0; i--) {
		LIST_INSERT_HEAD(&tls_entry_free_list, &tls_entries[i], link);
	}

	LIST_INIT(&thread_free_list);
	for (i = PTHREAD_THREADS_MAX - 1; i >= 1; i--) {
		LIST_INSERT_HEAD(&thread_free_list, &threads[i], link);
	}

	threads[0].env_id = syscall_getenvid();

	if (sem_init(&threads[0].sem, 0, 0) ||
		sem_init(&global_mutex, 0, 1)) {
		user_panic("pthread_init: could not create semaphores");
	}

	pthread_atfork(NULL, NULL, &pthread_reinit);
}

void _pthread_proc_exit() {
	pthread_t i, self;

	if (_pthread_disabled) {
		syscall_env_destroy(0, 1);
	}

	user_assert(!sem_wait(&global_mutex));

	// other threads may be waiting on semaphores when killed, so we need to
	// clear the wait list before calling sem_destroy

	self = pthread_self();
	for (i = 0; i < PTHREAD_THREADS_MAX; i++) {
		if (threads[i].env_id) {
			if (i != self && !(threads[i].flags & THREAD_ZOMBIE)) {
				syscall_env_destroy(threads[i].env_id, 0);
			}
			while (!sem_getvalue(&threads[i].sem)) {
				user_assert(!sem_post(&threads[i].sem));
			}
			user_assert(!sem_destroy(&threads[i].sem));
		}
	}

	while (!sem_getvalue(&global_mutex)) {
		user_assert(!sem_post(&global_mutex));
	}
	user_assert(!sem_destroy(&global_mutex));

	syscall_env_destroy(0, 1);
}

pthread_t pthread_self() {
	pthread_t ret;
	u_int env_id = syscall_getenvid();

	for (ret = 0; ret < PTHREAD_THREADS_MAX; ret++) {
		if (threads[ret].env_id == env_id) {
			return ret;
		}
	}

	user_panic("pthread_self: thread not found");
}

int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1 == t2;
}

int pthread_attr_init(pthread_attr_t *attr) {
	attr->detachstate = PTHREAD_CREATE_JOINABLE;
	attr->stacksize = PDMAP;
	return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
	return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	*detachstate = attr->detachstate;
	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	attr->detachstate = detachstate;
	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize) {
	*stacksize = attr->stacksize;
	return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	if (stacksize < PTHREAD_STACK_MIN || stacksize & 3) {
		return EINVAL;
	}
	attr->stacksize = stacksize;
	return 0;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *)) {
	pthread_t i;
	tls_entry_t *entry;

	user_assert(!sem_wait(&global_mutex));
	entry = LIST_FIRST(&tls_entry_free_list);
	if (!entry) {
		user_assert(!sem_post(&global_mutex));
		return EAGAIN;
	}
	LIST_REMOVE(entry, link);
	entry->dtor = destructor;
	for (i = 0; i < PTHREAD_THREADS_MAX; i++) {
		threads[i].tls[*key] = NULL;
	}
	user_assert(!sem_post(&global_mutex));

	*key = (pthread_key_t)(entry - tls_entries);
	return 0;
}

int pthread_key_delete(pthread_key_t key) {
	tls_entry_t *entry = &tls_entries[key];

	user_assert(!sem_wait(&global_mutex));
	entry->dtor = NULL;
	LIST_INSERT_HEAD(&tls_entry_free_list, entry, link);
	user_assert(!sem_post(&global_mutex));

	return 0;
}

void *pthread_getspecific(pthread_key_t key) {
	thread_t *thread = &threads[pthread_self()];

	return thread->tls[key];
}

int pthread_setspecific(pthread_key_t key, const void *value) {
	thread_t *thread = &threads[pthread_self()];

	thread->tls[key] = (void *)value;
	return 0;
}

int pthread_setcancelstate(int state, int *oldstate) {
	thread_t *thread = &threads[pthread_self()];

	if (oldstate) {
		*oldstate = thread->cancel_state;
	}
	thread->cancel_state = state;

	if (state == PTHREAD_CANCEL_ENABLE) {
		pthread_testcancel();
	}

	return 0;
}

// async cancellation is not supported
int pthread_setcanceltype(int type, int *oldtype) {
	thread_t *thread = &threads[pthread_self()];

	if (oldtype) {
		*oldtype = thread->cancel_type;
	}
	thread->cancel_type = type;
	return 0;
}

void pthread_testcancel() {
	thread_t *thread = &threads[pthread_self()];

	if (thread->cancel_state == PTHREAD_CANCEL_DISABLE) {
		return;
	}

	if (thread->flags & THREAD_CANCEL_PENDING) {
		pthread_exit(PTHREAD_CANCELED);
	}
}

static void thread_main(void *arg0, void *arg1) {
	void *(*fn)(void *) = (void *(*)(void *))arg0;

	pthread_exit(fn(arg1));
}

int pthread_create(
	pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine)(void *), void *arg) {
	thread_t *entry;
	void *xstacktop;
	int r;

	user_assert(!sem_wait(&global_mutex));

	entry = LIST_FIRST(&thread_free_list);
	if (!entry) {
		user_assert(!sem_post(&global_mutex));
		return EAGAIN;
	}
	LIST_REMOVE(entry, link);

	user_bzero(entry, sizeof(thread_t));
	if (sem_init(&entry->sem, 0, 0)) {
		user_panic("pthread_create: could not create thread semaphore");
	}
	if (attr->detachstate == PTHREAD_CREATE_DETACHED) {
		entry->flags |= THREAD_DETACHED;
	}

	// stacksize is ignored
	xstacktop = (void *)(UTOP - *thread * PDMAP);
	if (syscall_mem_alloc(0, (u_int)(xstacktop - BY2PG), PTE_V | PTE_R) < 0) {
		user_panic("pthread_create: could not allocate exception stack");
	}
	if (syscall_mem_alloc(0, (u_int)(xstacktop - 3 * BY2PG), PTE_V | PTE_R) < 0) {
		user_panic("pthread_create: could not allocate stack");
	}
	if ((r = syscall_env_alloc2(&thread_main, xstacktop - 2 * BY2PG, (void *)start_routine, arg)) < 0) {
		return r;
	}
	entry->env_id = (u_int)r;

	if (syscall_set_pgfault_handler(entry->env_id, __asm_pgfault_handler, (u_int)xstacktop) < 0) {
		user_panic("pthread_create: could not set page fault handler");
	}
	if (syscall_set_env_status(entry->env_id, ENV_RUNNABLE) < 0) {
		user_panic("pthread_create: could not start the thread");
	}

	user_assert(!sem_post(&global_mutex));

	*thread = (pthread_t)(entry - threads);
	return 0;
}

static void thread_free_unlocked(thread_t *thread) {
	user_assert(!sem_destroy(&thread->sem));
	thread->env_id = 0;
	LIST_INSERT_HEAD(&thread_free_list, thread, link);
}

static void thread_free(thread_t *thread) {
	user_assert(!sem_wait(&global_mutex));
	thread_free_unlocked(thread);
	user_assert(!sem_post(&global_mutex));
}

void pthread_exit(void *value_ptr) {
	thread_t *thread = &threads[pthread_self()];
	pthread_key_t i;
	pthread_t j;
	void (*dtor)(void *);
	void *val;
	int last_thread;

	for (i = 0; i < PTHREAD_KEYS_MAX; i++) {
		user_assert(!sem_wait(&global_mutex));
		dtor = tls_entries[i].dtor;
		val = thread->tls[i];
		thread->tls[i] = NULL;
		user_assert(!sem_post(&global_mutex));

		if (dtor && val) {
			dtor(val);
		}
	}

	user_assert(!sem_wait(&global_mutex));

	last_thread = 1;
	for (j = 0; j < PTHREAD_THREADS_MAX; j++) {
		if (threads[j].env_id && threads[j].env_id != thread->env_id &&
			!(threads[j].flags & THREAD_ZOMBIE)) {
			last_thread = 0;
		}
	}
	if (last_thread) {
		user_assert(!sem_post(&global_mutex));
		_pthread_proc_exit();
	}

	if (thread->flags & THREAD_DETACHED) {
		thread_free_unlocked(thread);
	} else {
		thread->exit_status = value_ptr;
		thread->flags |= THREAD_ZOMBIE;
		user_assert(!sem_post(&thread->sem));
	}

	user_assert(!sem_post(&global_mutex));

	syscall_env_destroy(0, 0);
}

int pthread_cancel(pthread_t thread) {
	thread_t *entry = &threads[thread];

	user_assert(!sem_wait(&global_mutex));
	entry->flags |= THREAD_CANCEL_PENDING;
	user_assert(!sem_post(&global_mutex));

	return 0;
}

int pthread_detach(pthread_t thread) {
	thread_t *entry = &threads[thread];

	user_assert(!sem_wait(&global_mutex));
	if (entry->flags & THREAD_ZOMBIE) {
		thread_free_unlocked(entry);
	} else {
		entry->flags |= THREAD_DETACHED;
	}
	user_assert(!sem_post(&global_mutex));

	return 0;
}

int pthread_join(pthread_t thread, void **value_ptr) {
	thread_t *entry = &threads[thread];

	user_assert(!sem_wait(&entry->sem));
	if (value_ptr) {
		*value_ptr = entry->exit_status;
	}
	thread_free(entry);
	return 0;
}

int pthread_atfork(void (*prepare)(), void (*parent)(), void (*child)()) {
	user_assert(!sem_wait(&global_mutex));
	if (_pthread_atfork_count == _PTHREAD_ATFORK_MAX) {
		user_assert(!sem_post(&global_mutex));
		return ENOMEM;
	}
	_pthread_prepare_hooks[_pthread_atfork_count] = prepare;
	_pthread_parent_hooks[_pthread_atfork_count] = parent;
	_pthread_child_hooks[_pthread_atfork_count] = child;
	// must increase count at last because fork() doesn't acquire the lock when
	// reading the hooks
	_pthread_atfork_count++;
	user_assert(!sem_post(&global_mutex));

	return 0;
}
