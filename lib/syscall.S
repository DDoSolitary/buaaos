#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <stackframe.h>
#include <unistd.h>

/*** exercise 4.2 ***/
NESTED(handle_sys,TF_SIZE, sp)
    SAVE_ALL                            // Macro used to save trapframe
    CLI                                 // Clean Interrupt Mask
    nop
    .set at                             // Resume use of $at

    // TODO: Fetch EPC from Trapframe, calculate a proper value and store it back to trapframe.
    lw t0, TF_EPC(sp)
    addiu t0, t0, 4
    sw t0, TF_EPC(sp)

    // TODO: Copy the syscall number into $a0.
    
    addiu   a0, a0, -__SYSCALL_BASE     // a0 <- relative syscall number
    sll     t0, a0, 2                   // t0 <- relative syscall number times 4
    la      t1, sys_call_table          // t1 <- syscall table base
    addu    t1, t1, t0                  // t1 <- table entry of specific syscall
    lw      t2, 0(t1)                   // t2 <- function entry of specific syscall

    lw t0, TF_REG29(sp)
    move s0, sp
    LOOP_BEGIN:
    ble a1, 3, LOOP_END
    nop
    addiu a1, a1, -1
    addiu t1, a1, 2
    sll t1, t1, 2
    addu t1, t1, t0
    lw t1, (t1)
    addiu sp, sp, -4
    sw t1, (sp)
    b LOOP_BEGIN
    nop
    LOOP_END:
    addiu sp, sp, -16
    move t1, a1
    move a1, a2
    move a2, a3
    bne t1, 3, SKIP_A3
    nop
    lw a3, 16(t0)
    SKIP_A3:

    jalr    t2                          // Invoke sys_* function
    nop
    
    // TODO: Resume current kernel stack
    move sp, s0
    
    sw      v0, TF_REG2(sp)             // Store return value of function sys_* (in $v0) into trapframe

    j       ret_from_exception          // Return from exeception
    nop
END(handle_sys)

sys_call_table:                         // Syscall Table
.align 2
    .word sys_putchar
    .word sys_getenvid
    .word sys_yield
    .word sys_env_destroy
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_env_alloc
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_cgetc
    .word sys_ipc_can_multi_send
