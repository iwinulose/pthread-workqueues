//Charles Duyk
//charles.duyk@gmail.com
//John Stumpo
//stump@jstump.com
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "psem.h"

void psem_init(psem_t *semaphore, int value) {
	//FIXME error checking (........)
	pthread_mutex_init(&semaphore->lock, NULL);
	pthread_cond_init(&semaphore->condvar, NULL); 
	semaphore->counter = value;
}

psem_t * psem_new(int value) {
	psem_t *ret = calloc(1, sizeof(psem_t));
	if(ret == NULL) {
		errno = ENOMEM;
	}
	else {
		psem_init(ret, value);
	}
	return ret;
}

void psem_free(psem_t *semaphore) {
	free(semaphore);
}

void psem_up(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	if(++(semaphore->counter) <= 0) {
		pthread_cond_signal(&semaphore->condvar);
	}
	pthread_mutex_unlock(&semaphore->lock);
}

void psem_down(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	if(--(semaphore->counter) < 0) {
		pthread_cond_wait(&semaphore->condvar, &semaphore->lock); 
	}
	pthread_mutex_unlock(&semaphore->lock);
}

int psem_peek(psem_t *semaphore) {
	//returns the value in the semaphore. Because we cant use this for "real" decisions using this information anyway, we don't bother taking the lock.
	return semaphore->counter;
}
