#include <print.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

struct s1 {
    int a;
    char b;
    char c;
    int d;
};

struct s2 {
    int size;
    int c[1];
};

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
    struct s1 ts1;
    ts1.a = 123;
    ts1.b = 'a';
    ts1.c = '0';
    ts1.d = -10;
    test_print("{123,a,0,-10}", "%$1T", &ts1);
    test_print("{0123,   a,   0,-010}", "%04$1T", &ts1);
    test_print("{0000000123,         a,         0,-000000010}", "%010.12341l$1T", &ts1);

    struct s2 *ps2 = malloc(sizeof(int) * 4);
    ps2->size = 3;
    ps2->c[0] = 123;
    ps2->c[1] = -10;
    ps2->c[2] = 2147483647;
    test_print("{3,123,-10,2147483647}", "%$2T", ps2);
    test_print("{0003,0123,-010,2147483647}", "%04$2T", ps2);
    test_print("{0000000003,0000000123,-000000010,2147483647}", "%010.452$2T", ps2);
}
