#include <asm/regdef.h>
#include <asm/asm.h>

.text

LEAF(_my_putchar)
	.set mips2
	.set reorder
	sb a0, 0xb0000000
	j ra
	nop
END(_my_putchar)

LEAF(_my_getchar)
	.set mips2
	.set reorder
	L0:
	lb v0, 0xb0000000
	beqz v0, L0
	nop
	sb v0, 0xb0000000
	beq v0, 13, L1
	nop
	li t0, 10
	sb t0, 0xb0000000
	L1:
	j ra
	nop
END(_my_getchar)

LEAF(_my_exit)
	.set mips2
	.set reorder
	sb zero, 0xb0000001
	j ra
	nop
END(_my_exit)
