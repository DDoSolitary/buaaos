#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

.text
LEAF(kcons_init)
mfc0 t0, CP0_STATUS
or t0, STATUS_CU0 | 0x401
mtc0 t0, CP0_STATUS
END(kcons_init)
