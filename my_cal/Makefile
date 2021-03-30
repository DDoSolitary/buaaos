INCLUDES := -I../include/

.PHONY: all clean

all: my_driver.o my_cal.o

my_driver.o: my_driver.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

my_cal.o: my_cal.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 

clean:
	rm -rf *.o

include ../include.mk
