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

#include "stdincludes.h"
#include <pthread.h>
#include "pthread_wq_np.h"
#define _POSIX_C_SOURCE  200000L
#include <time.h>

static pthread_workqueue_t wq;
static volatile int count = 0;

static void * the_freeing_function(void *arg) {
	int *ip = (int *) arg;
	printf("FREE: Hey hi freeing %p (%d)\n", arg, *ip);
	free(arg);
	return NULL;
}

static void * the_function(void *arg) {
	pthread_workitem_handle_t handle;
	unsigned int gencount;
	int *ip = (int *) arg;
	printf("Hey hi #%d from a workqueue. Arg is at %p!\n", *ip, arg);
	pthread_workqueue_additem_np(wq, the_freeing_function, arg, &handle, &gencount);
	return NULL;
}

int mutual_post_test(void) {
	pthread_workitem_handle_t handle;
	unsigned int gencount;
loop:
	for(int i = 0; i < 50; i++) {
		printf("Inserting %d gen\n", i);
		int *ip = malloc(sizeof(int));
		*ip = i;
		pthread_workqueue_additem_np(wq, the_function, ip, &handle, &gencount);
		if((i%25) == 0) {
			sleep(2);
		}
	}
	sleep(10);
	goto loop;
	return 0;
}

int main(void) {
	pthread_workqueue_init_np();
	pthread_workqueue_create_np(&wq, NULL);
	return mutual_post_test();
}
