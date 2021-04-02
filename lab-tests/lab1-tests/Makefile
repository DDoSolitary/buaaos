CC := gcc
INCLUDES := -I../../include/

.PHONY: all clean test

all: lab1-tests

print.o: ../../lib/print.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $^

main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $^

lab1-tests: print.o main.o
	$(CC) -o $@ $^

clean:
	rm -rf lab1-tests *.o

test: lab1-tests
	./lab1-tests
