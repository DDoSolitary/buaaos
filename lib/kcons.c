#include <types.h>
#include <printf.h>

u_int sys_cgetc();
u_int sys_get_time(int sysno);

u_int last_time;
char buf[128 + 1], len;

void handle_cons_ir(u_int cp0_status) {
	u_int ts = sys_get_time(0);
	u_int diff = ts - last_time;
	buf[len++] = sys_cgetc();
	if (!last_time) {
		printf("CP0 STATUS: %x, 1st interrupt: %d\n", cp0_status, ts);
	} else if (diff < 5) {
		printf("interval: %d\n", diff);
	} else {
		buf[len] = '\0';
		printf("length=%d, string=%s\n", len, buf);
		*(volatile char *)0xb0000010 = 0;
	}
	last_time = ts;
}
