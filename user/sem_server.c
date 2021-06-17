#include "lib.h"
#include "semaphore.h"
#include <queue.h>

#define REQVA 0x0ffff000

#define SEM_OPEN 0
#define SEM_OPENIF 1
#define SEM_CREATE 2

typedef struct sem_wait {
	LIST_ENTRY(sem_wait) link;
	u_int env_id;
} sem_wait_t;

sem_wait_t sem_waits[SEM_WAIT_MAX + 1];

LIST_HEAD(sem_wait_head, sem_wait);
struct sem_wait_head sem_wait_free_list;

typedef struct sem_entry {
	LIST_ENTRY(sem_entry) link;
	u_short val;
	u_int ref;
	char name[SEM_NAME_MAX + 1];
	struct sem_wait_head wait_list;
} sem_entry_t;

sem_entry_t sems[SEM_NSEMS_MAX + 1];

LIST_HEAD(sem_head, sem_entry);
struct sem_head sem_free_list;

static void sem_svr_init() {
	int i;

	LIST_INIT(&sem_free_list);
	for (i = SEM_NSEMS_MAX; i >= 0; i--) {
		LIST_INSERT_HEAD(&sem_free_list, &sems[i], link);
	}

	LIST_INIT(&sem_wait_free_list);
	for (i = SEM_WAIT_MAX; i >= 0; i--) {
		LIST_INSERT_HEAD(&sem_wait_free_list, &sem_waits[i], link);
	}
}

static u_int sem_svr_alloc(u_short val) {
	sem_entry_t *sem = LIST_FIRST(&sem_free_list);

	if (!sem) {
		return (u_int)-1;
	}
	LIST_REMOVE(sem, link);
	sem->val = val;
	LIST_INIT(&sem->wait_list);
	return (u_int)(sem - sems);
}

static u_int sem_svr_free(sem_entry_t *sem) {
	if (!LIST_EMPTY(&sem->wait_list)) {
		return 1;
	}
	LIST_INSERT_HEAD(&sem_free_list, sem, link);
	return 0;
}

static int sem_svr_wait(sem_entry_t *sem, int block, u_int env_id) {
	sem_wait_t *sem_wait;

	if (sem->val > 0) {
		sem->val--;
		return 0;
	}
	if (!block) {
		return 1;
	}
	sem_wait = LIST_FIRST(&sem_wait_free_list);
	if (!sem_wait) {
		return 1;
	}
	LIST_REMOVE(sem_wait, link);
	sem_wait->env_id = env_id;
	LIST_INSERT_HEAD(&sem->wait_list, sem_wait, link);
	return -1;
}

static int sem_svr_post(sem_entry_t *sem) {
	sem_wait_t *sem_wait = LIST_FIRST(&sem->wait_list);

	if (!sem_wait) {
		sem->val++;
	} else {
		LIST_REMOVE(sem_wait, link);
		LIST_INSERT_HEAD(&sem_wait_free_list, sem_wait, link);
		ipc_send(sem_wait->env_id, 0, 0, 0);
	}
	return 0;
}

static u_int sem_svr_get(sem_entry_t *sem) {
	return sem->val;
}

static sem_entry_t *sem_svr_find(const char *name) {
	sem_entry_t *ret = NULL;
	u_int i;

	for (i = 0; i <= SEM_NSEMS_MAX; i++) {
		if (!strcmp(name, sems[i].name)) {
			ret = &sems[i];
			break;
		}
	}
	return ret;
}

static u_int sem_svr_open(const char *name, u_short val, int opt) {
	u_int r;
	sem_entry_t *sem = sem_svr_find(name);

	if (sem) {
		if (opt == SEM_CREATE) {
			return (u_int)-1;
		}
		return (u_int)(sem - sems);
	}
	if (opt == SEM_OPEN || strlen(name) > SEM_NAME_MAX) {
		return (u_int)-1;
	}
	r = sem_svr_alloc(val);
	if (r <= SEM_NSEMS_MAX) {
		strcpy(sems[r].name, name);
	}
	return r;
}

static u_int sem_svr_close(sem_entry_t *sem) {
	if (!--sem->ref && !sem->name[0]) {
		return sem_svr_free(sem);
	}
	return 0;
}

static u_int sem_svr_unlink(const char *name) {
	sem_entry_t *sem = sem_svr_find(name);

	if (!sem) {
		return 1;
	}
	sem->name[0] = '\0';
	if (!sem->ref) {
		return sem_svr_free(sem);
	}
	return 0;
}

void umain() {
	u_int req, env_id;
	u_short req_op, req_arg;
	int r;

	sem_svr_init();

	while (1) {
		req = ipc_recv(&env_id, REQVA, NULL);
		req_op = (u_short)req;
		req_arg = (u_short)(req >> 16);

		switch (req_op) {
		case SEMREQ_ALLOC:
			ipc_send(env_id, sem_svr_alloc(req_arg), 0, 0);
			break;
		case SEMREQ_FREE:
			ipc_send(env_id, sem_svr_free(&sems[req_arg]), 0, 0);
			break;
		case SEMREQ_WAIT:
		case SEMREQ_TRYWAIT:
			r = sem_svr_wait(&sems[req_arg], req_op == SEMREQ_WAIT, env_id);
			if (r >= 0) {
				ipc_send(env_id, (u_int)r, 0, 0);
			}
			break;
		case SEMREQ_POST:
			ipc_send(env_id, sem_svr_post(&sems[req_arg]), 0, 0);
			break;
		case SEMREQ_GET:
			ipc_send(env_id, sem_svr_get(&sems[req_arg]), 0, 0);
			break;
		case SEMREQ_OPEN:
			ipc_send(env_id, sem_svr_open((const char *)REQVA, req_arg, SEM_OPEN), 0, 0);
			break;
		case SEMREQ_OPENIF:
			ipc_send(env_id, sem_svr_open((const char *)REQVA, req_arg, SEM_OPENIF), 0, 0);
			break;
		case SEMREQ_CREATE:
			ipc_send(env_id, sem_svr_open((const char *)REQVA, req_arg, SEM_CREATE), 0, 0);
			break;
		case SEMREQ_CLOSE:
			ipc_send(env_id, sem_svr_close(&sems[req_arg]), 0, 0);
			break;
		case SEMREQ_UNLINK:
			ipc_send(env_id, sem_svr_unlink((const char *)REQVA), 0, 0);
			break;
		default:
			ipc_send(env_id, 1, 0, 0);
			break;
		}
	}
}
