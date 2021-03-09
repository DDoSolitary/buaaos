all: fibo
code/main.o: code/main.c
	gcc -c -I include -o code/main.o code/main.c
code/fibo.o: code/fibo.c
	gcc -c -I include -o code/fibo.o code/fibo.c
fibo: code/main.o code/fibo.o
	gcc -o fibo code/main.o code/fibo.o
clean:
	rm -f code/main.o code/fibo.o
