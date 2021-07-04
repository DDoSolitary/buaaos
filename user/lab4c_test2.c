#include "lib.h"
#include "semaphore.h"

void umain() {
	u_int envid = syscall_getenvid();
	sem_t *sem = sem_open("/sem_test", O_CREAT, 0, 1);
	user_assert(sem);
	if (!sem_trywait(sem)) {
		writef("lab4c_test: env %08x started successfully\n", envid);
		while (1);
	} else {
		writef("lab4c_test: env %08x not started because program is already running\n", envid);
	}
}
