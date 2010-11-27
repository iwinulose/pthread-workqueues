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

static volatile int _wq_configured = 0;
static pthread_mutex_t _init_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t _job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static psem_t _job_semaphore;
static dequeue_t *_high_prio_job_queue = NULL;
static dequeue_t *_normal_prio_job_queue = NULL;
static dequeue_t *_low_prio_job_queue = NULL;


#define IS_VALID_QUEUE_PRIORITY(x) (((x) == WORKQ_HIGH_PRIOQUEUE) || ((x) == WORKQ_DEFAULT_PRIOQUEUE) || ((x) == WORKQ_LOW_PRIOQUEUE))

const pthread_workqueue_attr_t _default_workqueue_attributes = {
	.sig		= PTHREAD_WORKQUEUE_ATTR_T_SIG,
	.priority 	= WORKQ_DEFAULT_PRIOQUEUE,
	.overcommit = 0,
};

static int _is_valid_attr(pthread_workqueue_attr_t *attr) {
	int ret = 0;
	if(attr == NULL) {
		ret = 0;
	}
	else if(attr->sig != PTHREAD_WORKQUEUE_ATTR_T_SIG) {
		ret = 0;
	}
	else {
		ret = 1;
	}
	return ret;
}

static int _is_valid_workqueue(pthread_workqueue_t *workqueue) {
	int ret = 0;
	if(workqueue == NULL) {
		ret = 0;
	}
	else if(workqueue->sig != PTHREAD_WORKQUEUE_T_SIG) {
		ret = 0;
	}
	else{
		ret = 1;
	}
	return ret;
}

int pthread_workqueue_init_np(void) {
	//pthread_once would be we want here but for the fact that we can't get the correct behavior out of it--we can't get state out of the init routine outside of a global and ensuring the proper behavior would require external synchronization.
	int ret = -1; //should never be returned
	pthread_mutex_lock(&_init_mutex);
	if(!_wq_configured) {
		printf("configuring\n");
		//setup
		psem_init(&_job_semaphore, 0);
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

int pthread_workqueue_create_np(pthread_workqueue_t *workqp, const pthread_workqueue_attr_t * attrp) {
	int ret = -1;
	pthread_workqueue_attr_t attr;
	if(_is_valid_workqueue(workqp)) {
		if(attrp != NULL) {
			attr = *attrp;
		}
		else {
			attr = _default_workqueue_attributes;
		}
	}	
	return ret;
}

int pthread_workqueue_additem_np(pthread_workqueue_t workq, void *(*workitem_func)(void *), void * workitem_arg, pthread_workitem_handle_t * itemhandlep, unsigned int *gencountp) {
	int ret = -1;
	if(_is_valid_workqueue(&workq)) {
	}
	return 0;
}

int pthread_workqueue_attr_init_np(pthread_workqueue_attr_t *attr) {
	int ret = -1;
	if(attr == NULL) {
		ret = EINVAL;
	}
	else {
		attr->sig = PTHREAD_WORKQUEUE_ATTR_T_SIG;
		attr->priority = WORKQ_DEFAULT_PRIOQUEUE;
		attr->overcommit = 0;
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_destroy_np(pthread_workqueue_attr_t *attr) {
	(void) attr;
	return 0;
}

int pthread_workqueue_attr_getovercommit_np(pthread_workqueue_attr_t *attr, int *ocommp) {
	int ret = -1;
	if(!_is_valid_attr(attr) || ocommp == NULL) {
		ret = EINVAL;
	}
	else {
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_setovercommit_np(pthread_workqueue_attr_t *attr, int ocomm) {
	int ret = EINVAL;
	if(_is_valid_attr(attr)) {
		attr->overcommit = ocomm;
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_getqueuepriority_np(pthread_workqueue_attr_t *attr, int *qpriop) {
	int ret = -1;
	if (!_is_valid_attr(attr) || qpriop == NULL) {
		ret = EINVAL;
	}
	else {
		*qpriop = attr->priority;
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_setqueuepriority_np(pthread_workqueue_attr_t *attr, int qprio) {
	int ret = -1;
	if(_is_valid_attr(attr) && IS_VALID_QUEUE_PRIORITY(qprio)) {
		attr->priority = qprio;
		ret = 0;
	}
	else {
		ret = EINVAL;
	}
	return ret;
}
