#include "lib.h"
#include "semaphore.h"
#include "pthread.h"

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];

sem_t free, used, mutex;

int get() {
	int i, ret;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (buffer[i]) {
			ret = buffer[i];
			buffer[i] = 0;
			writef("got %d from slot %d\n", ret, i);
			return ret;
		}
	}
	user_panic("get: buffer empty");
}

void put(int val) {
	int i;
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (!buffer[i]) {
			buffer[i] = val;
			writef("put %d to slot %d\n", val, i);
			return;
		}
	}
	user_panic("put: buffer full");
}

void *produce(void *arg) {
	size_t count = (size_t)arg, i;
	for (i = 1; i <= count; i++) {
		user_assert(!sem_wait(&free));
		user_assert(!sem_wait(&mutex));
		put(i);
		user_assert(!sem_post(&mutex));
		user_assert(!sem_post(&used));
	}
	return (void *)count;
}

void *consume(void *arg) {
	size_t count = (size_t)arg, sum = 0, i;
	for (i = 0; i < count; i++) {
		user_assert(!sem_wait(&used));
		user_assert(!sem_wait(&mutex));
		sum += get();
		user_assert(!sem_post(&mutex));
		user_assert(!sem_post(&free));
	}
	return (void *)sum;
}

void umain() {
	user_assert(!sem_init(&free, 0, BUFFER_SIZE));
	user_assert(!sem_init(&used, 0, 0));
	user_assert(!sem_init(&mutex, 0, 1));
	
	pthread_t producer, consumer;
	user_assert(!pthread_create(&consumer, NULL, &consume, (void *)100));
	user_assert(!pthread_create(&producer, NULL, &produce, (void *)100));
	
	void *ret;
	user_assert(!pthread_join(producer, &ret));
	writef("producer: %u\n", (size_t)ret);
	user_assert(!pthread_join(consumer, &ret));
	writef("consumer: %u\n", (size_t)ret);

	user_assert(!sem_destroy(&free));
	user_assert(!sem_destroy(&used));
	user_assert(!sem_destroy(&mutex));
}
