#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "psem.h"

void sem_init(psem_t *semaphore, int value) {
	//FIXME error checking (........)
	pthread_mutex_init(&semaphore->lock, NULL);
	pthread_cond_init(&semaphore->condvar, NULL); 
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
	if(++(semaphore->counter) <= 0) {
		pthread_cond_signal(&semaphore->condvar);
	}
	pthread_mutex_unlock(&semaphore->lock);
}

void sem_down(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	if(--(semaphore->counter) < 0) {
		pthread_cond_wait(&semaphore->condvar, &semaphore->lock); 
	}
	pthread_mutex_unlock(&semaphore->lock);
}
