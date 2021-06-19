// User-level page fault handler.  We use an assembly wrapper around a C function.
// The assembly wrapper is in entry.S.

#include "lib.h"
#include <mmu.h>

extern void (*__pgfault_handler)(u_int);

u_int get_xstacktop() {
	u_int ret;
	
	ret = ROUND((u_int)&ret, PDMAP);
	return ret;
}

//
// Set the page fault handler function.
// If there isn't one yet, _pgfault_handler will be 0.
// The first time we register a handler, we need to
// allocate an exception stack and tell the kernel to
// call _asm_pgfault_handler on it.
//
void
set_pgfault_handler(void (*fn)(u_int va))
{
	u_int xstacktop = get_xstacktop();

	if (__pgfault_handler == 0) {
		// Your code here:
		// map one page of exception stack with top at UXSTACKTOP
		// register assembly handler and stack with operating system
		if (syscall_mem_alloc(0, xstacktop - BY2PG, PTE_V | PTE_R) < 0 ||
			syscall_set_pgfault_handler(0, __asm_pgfault_handler, xstacktop) < 0) {
			writef("cannot set pgfault handler\n");
			return;
		}

		//		panic("set_pgfault_handler not implemented");
	}

	// Save handler pointer for assembly to call.
	__pgfault_handler = fn;
}

