#include <pthread.h>
#include <errno.h>
#include "psem.h"

struct psem_s {
	pthread_mutex_t lock;
	pthread_cond_t condvar;
	int counter;
};

void sem_init(psem_t *semaphore, int value) {
	semaphore->lock = PTHREAD_MUTEX_INITIALIZER;
	semaphore->condvar = PTHREAD_COND_INITIALIZER;
	semaphore->counter = value;
}

psem_t * sem_new(int value) {
	psem_t *ret = calloc(1, sizeof(psem_t));
	if(ret == NULL) {
		errno = ENOMEM;
	}
	else {
		sem_init(ret, value);
	}
	return ret;
}

void sem_free(psem_t *semaphore) {
	free(semaphore);
}

void sem_up(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	if(semaphore->++counter <= 0) {
		pthread_cond_signal(&semaphore->condvar);
	}
	pthread_mutex_unlock(semaphore->lock);
}

void sem_down(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	if(semaphore->--counter < 0) {
		pthread_cond_wait(&semaphore->condvar, &semaphore->lock); 
	}
	pthread_mutex_unlock(&semaphore->lock);
}
