CC := gcc
INCLUDES := -I../../include/
SRCS := main.c test-queue.c
OBJS := $(patsubst %.c, %.o, $(SRCS))

.PHONY: all clean test

all: lab2-tests

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $^

lab2-tests: $(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf lab2-tests $(OBJS)

test: lab2-tests
	./lab2-tests
