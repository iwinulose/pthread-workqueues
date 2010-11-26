#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include "pthread_wq_np.h"
#include "pthread_wq_np_priv.h"
#include "dequeue.h"
#include "psem.h"


#include <unistd.h>

static volatile int _wq_configured = 0;
static pthread_mutex_t _init_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t _job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static psem_t _job_semaphore;
static dequeue_t *_high_prio_job_queue = NULL;
static dequeue_t *_normal_prio_job_queue = NULL;
static dequeue_t *_low_prio_job_queue = NULL;

int pthread_workqueue_init_np(void) {
	//pthread_once would be we want here but for the fact that we can't get the correct behavior out of it--we can't get state out of the init routine outside of a global and ensuring the proper behavior would require external synchronization.
	int ret = -1; //should never be returned
	pthread_mutex_lock(&_init_mutex);
	if(!_wq_configured) {
		printf("configuring\n");
		//setup
		sem_init(&_job_semaphore, 0);
		_high_prio_job_queue 	= dequeue_new();
		_normal_prio_job_queue 	= dequeue_new();
		_low_prio_job_queue		= dequeue_new();
		_wq_configured = 1;
		ret = 0;
	}
	else {
		ret = 0;
	}
	pthread_mutex_unlock(&_init_mutex);
	return ret;
}

int pthread_workqueue_create_np(pthread_workqueue_t *workqp, const pthread_workqueue_attr_t * attr) {
	return 0;
}

int pthread_workqueue_additem_np(pthread_workqueue_t workq, void *(*workitem_func)(void *), void * workitem_arg, pthread_workitem_handle_t * itemhandlep, unsigned int *gencountp) {
	return 0;
}

