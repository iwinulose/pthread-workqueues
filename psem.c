/* Copyright (c) 2010 Charles Duyk and John Stumpo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the names of the authors nor the names of other contributors
 *        may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

int psem_down_timed(psem_t *semaphore, struct timeval *time) {
	int ret = 0;
	pthread_mutex_lock(&semaphore->lock);
	if(--(semaphore->counter) < 0) {
		ret = pthread_cond_timedwait(&semaphore->condvar, &semaphore->lock, time); 
		if(ret == ETIMEDOUT) {
			++(semaphore->counter);
		}
	}
	pthread_mutex_unlock(&semaphore->lock);
	return ret;
}

int psem_peek(psem_t *semaphore) {
	pthread_mutex_lock(&semaphore->lock);
	int ret = semaphore->counter;
	pthread_mutex_unlock(&semaphore->lock);
	return ret;
}
