#include <stdio.h>

char digits[5];

int main() {
	int x, n = 0, i;
	scanf("%d", &x);
	do {
		digits[n++] = x % 10;
		x /= 10;
	} while (x != 0);
	for (i = 0; i < n / 2; i++) {
		if (digits[i] != digits[n - i - 1]) {
			puts("N");
			return 0;
		}
	}
	puts("Y");
	return 0;
}
