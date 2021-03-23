CC := gcc
INCLUDES := -I../../include/

.PHONY: all clean

all: test

print.o: ../print.c
	$(CC) $(INCLUDES) -o $@ -c $^

test.o: test.c
	$(CC) $(INCLUDES) -o $@ -c $^

test: print.o test.o
	$(CC) -o $@ $^

clean:
	rm -rf test *.o
