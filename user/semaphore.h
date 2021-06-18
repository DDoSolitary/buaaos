#ifndef _USER_SEMAPHORE_H_
#define _USER_SEMAPHORE_H_

#define SEMREQ_ALLOC 0
#define SEMREQ_FREE 1
#define SEMREQ_WAIT 2
#define SEMREQ_TRYWAIT 3
#define SEMREQ_POST 4
#define SEMREQ_GET 5
#define SEMREQ_OPEN 6
#define SEMREQ_OPENIF 7
#define SEMREQ_CREATE 8
#define SEMREQ_CLOSE 9
#define SEMREQ_UNLINK 10

#define SEM_NSEMS_MAX 1024
#define SEM_VALUE_MAX 65535
#define SEM_FAILED NULL

#define SEM_WAIT_MAX 1024
#define SEM_NAME_MAX 256

typedef u_short sem_t;
typedef u_int mode_t;

int sem_init(sem_t *sem, int pshared, unsigned value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem);
sem_t *sem_open(const char *name, int oflag, ...);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);

#endif
