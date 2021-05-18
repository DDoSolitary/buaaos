#include "lib.h"
#include <mmu.h>
#include <env.h>

void
exit(void)
{
	//close_all();
	syscall_env_destroy(0);
}


struct Env **env;

void
libmain(int argc, char **argv)
{
	struct Env *e;
	int envid;
	envid = syscall_getenvid();
	envid = ENVX(envid);
	e = &envs[envid];
	env = &e;
	// call user main routine
	umain(argc, argv);
	// exit gracefully
	exit();
	//syscall_env_destroy(0);
}
