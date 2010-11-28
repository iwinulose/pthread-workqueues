#include "stdincludes.h"
#include <pthread.h>
#include "pthread_wq_np.h"
#define _POSIX_C_SOURCE  200000L
#include <time.h>

pthread_workqueue_t wq;

void * the_freeing_function(void *arg) {
	int *ip = (int *) arg;
	printf("FREE: Hey hi freeing %p (%d)\n", arg, *ip);
	free(arg);
	return NULL;
}

void * the_function(void *arg) {
	pthread_workitem_handle_t handle;
	unsigned int gencount;
	int *ip = (int *) arg;
	printf("Hey hi #%d from a workqueue. Arg is at %p!\n", *ip, arg);
	pthread_workqueue_additem_np(wq, the_freeing_function, arg, &handle, &gencount);
	return NULL;
}

int main(void) {
	pthread_workqueue_init_np();
	pthread_workqueue_create_np(&wq, NULL);
	pthread_workitem_handle_t handle;
	unsigned int gencount;
	for(int i = 0; i < 50; i++) {
		printf("Inserting %d gen\n", i);
		int *ip = malloc(sizeof(int));
		*ip = i;
		pthread_workqueue_additem_np(wq, the_function, ip, &handle, &gencount);
		if((i%25) == 0) {
			sleep(1);
		}
	}
	while(1);
	return EXIT_SUCCESS;
}
