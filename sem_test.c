//Charles Duyk
//charles.duyk@gmail.com
//John Stumpo
//stump@jstump.com
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include "psem.h"

static psem_t _mutex;

void * run(void *arg) {
	(void)arg;
	unsigned int id = (unsigned int) pthread_self();
	printf("Thread %u initialized!\n", id);
	while(1) {
		sleep(1);
		printf("Thread %u going into mutex\n", id);
		psem_down(&_mutex);
		printf("Hello from thread %u!\n", id);
		sleep(3);
		psem_up(&_mutex);
	}
}

int main(void) {
	psem_init(&_mutex, 1);
	for(int i = 0; i < 5; i++) {
		pthread_t id;
		pthread_create(&id, NULL, run, NULL);
	}
	while(1);
}
