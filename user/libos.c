#include "lib.h"
#include "pthread.h"
#include <mmu.h>
#include <env.h>

void
exit(void)
{
	//close_all();
	_pthread_proc_exit();
}

void
libmain(int argc, char **argv)
{
	_pthread_init();
	// call user main routine
	umain(argc, argv);
	// exit gracefully
	exit();
	//syscall_env_destroy(0);
}
