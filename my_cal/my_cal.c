extern char _my_getchar();
extern void _my_putchar(char);
extern void _my_exit();

int read_int() {
	int x = 0;
	char c = _my_getchar();
	do {
		x = x * 10 + c - '0';
		c = _my_getchar();
	} while (c >= '0' && c <= '9');
	return x;
}

void write_int(int x) {
	char buf[10];
	int s = 0;
	do {
		buf[s++] = (char)(x % 10 + '0');
		x /= 10;
	} while (x > 0);
	while (s--) {
		_my_putchar(buf[s]);
	}
}

void my_cal() {
	int x = read_int();
	int y = read_int();
	write_int(x + y);
	_my_exit();
}
