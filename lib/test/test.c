#include <print.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char output_buf[65536];
int output_len;

void output(void *arg, char *s, int len) {
    strncpy(output_buf + output_len, s, len);
    output_len += len;
}

void test_print(char *expected, char *fmt, ...) {
    va_list li;
    va_start(li, fmt);
    output_len = 0;
    lp_Print(output, NULL, fmt, li);
    va_end(li);
    output_buf[output_len] = 0;
    if (strcmp(output_buf, expected)) {
	printf("test failed!\nexpected: %s\nfound: %s\n\n", expected, output_buf);
    }
}

int main() {
    test_print("foo 123", "%s %d", "foo", 123);
}
