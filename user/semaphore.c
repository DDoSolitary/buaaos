#include "lib.h"
#include "semaphore.h"
#include <stdarg.h>

extern char semipcbuf[BY2PG];

sem_t named_sems[SEM_NSEMS_MAX];

static u_int semipc(u_short req, u_short arg) {
	ipc_send(envs[0].env_id, ((u_int)arg << 16) | req, (u_int)&semipcbuf, PTE_V);
	return ipc_recv(NULL, 0, NULL);
}

// pshared is ignored
int sem_init(sem_t *sem, int pshared, unsigned value) {
	u_int r;

	if (value > SEM_VALUE_MAX) {
		return -1;
	}
	r = semipc(SEMREQ_ALLOC, (u_short)value);
	if (r >= SEM_NSEMS_MAX) {
		return -1;
	}
	*sem = r;
	return 0;
}

int sem_destroy(sem_t *sem) {
	u_int r = semipc(SEMREQ_FREE, *sem);
	return r ? -1 : 0;
}

int sem_wait(sem_t *sem) {
	u_int r = semipc(SEMREQ_WAIT, *sem);
	return r ? -1 : 0;
}

int sem_trywait(sem_t *sem) {
	u_int r = semipc(SEMREQ_TRYWAIT, *sem);
	return r ? -1 : 0;
}

int sem_post(sem_t *sem) {
	u_int r = semipc(SEMREQ_POST, *sem);
	return r ? -1 : 0;
}

int sem_getvalue(sem_t *sem) {
	u_int r = semipc(SEMREQ_GET, *sem);
	return r > SEM_VALUE_MAX ? -1 : (int)r;
}

sem_t *sem_open(const char *name, int oflag, ...) {
	u_int r;
	u_short req, arg;

	if (strlen(name) > SEM_NAME_MAX) {
		return SEM_FAILED;
	}
	strcpy(semipcbuf, name);

	if (oflag & O_CREAT) {
		if (oflag & O_EXCL) {
			req = SEMREQ_CREATE;
		} else {
			req = SEMREQ_OPENIF;
		}
		va_list ap;
		va_start(ap, oflag);
		// mode is ignored
		va_arg(ap, mode_t);
		arg = (u_short)va_arg(ap, unsigned);
		va_end(ap);
	} else {
		req = SEMREQ_OPEN;
		arg = 0;
	}

	r = semipc(req, arg);
	if (r >= SEM_NSEMS_MAX) {
		return SEM_FAILED;
	}
	named_sems[r] = (sem_t)r;
	return &named_sems[r];
}

int sem_close(sem_t *sem) {
	u_int r = semipc(SEMREQ_CLOSE, *sem);
	return r ? -1 : 0;
}

int sem_unlink(const char *name) {
	u_int r;

	if (strlen(name) > SEM_NAME_MAX) {
		return -1;
	}
	strcpy(semipcbuf, name);
	r = semipc(SEMREQ_UNLINK, 0);
	return r ? -1 : 0;
}
