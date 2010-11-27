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
static dequeue_t *_default_prio_job_queue = NULL;
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
	else if(!IS_VALID_QUEUE_PRIORITY(attr->priority)) {
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
	else if(!_is_valid_attr(&workqueue->attr)) {
		ret = 0;
	}
	else{
		ret = 1;
	}
	return ret;
}

static dequeue_t * _queue_for_priority(int priority) {
	dequeue_t *queue;
	switch(priority) {
		case WORKQ_HIGH_PRIOQUEUE:
			queue = _high_prio_job_queue;
			break;
		case WORKQ_DEFAULT_PRIOQUEUE:
			queue = _default_prio_job_queue;
			break;
		case WORKQ_LOW_PRIOQUEUE:
			queue = _low_prio_job_queue;
			break;
		default:
			queue = NULL;
			break;
	}
	return queue;
}

static void * _workqueue_worker(void *arg) {
	(void) arg;
	int running = 1;
	while(running) {
		psem_down(&_job_semaphore);
		pthread_workitem_handle_t *job = NULL;
		pthread_mutex_lock(&_job_queue_mutex);
		job = dequeue_pop(_high_prio_job_queue);
		if(job != NULL) {
			goto worker_unlock;
		}
		job = dequeue_pop(_default_prio_job_queue);
		if(job != NULL) {
			goto worker_unlock;
		}
		job = dequeue_pop(_high_prio_job_queue);
		if(job != NULL) {
			goto worker_unlock;
		}
worker_unlock:
		pthread_mutex_unlock(&_job_queue_mutex);
		if(job != NULL) {
			workitem_f func = job->func;
			void * arg		= job->arg;
			func(arg);
			free(job);
		}
	}
	return NULL;
}

static void _free_workqueues(void) {
	dequeue_free(_high_prio_job_queue);
	dequeue_free(_default_prio_job_queue);
	dequeue_free(_low_prio_job_queue);
}

int pthread_workqueue_init_np(void) {
	int ret = -1; //should never be returned
	pthread_mutex_lock(&_init_mutex);
	pthread_t thread;
	if(!_wq_configured) {
		psem_init(&_job_semaphore, 0);
		//FIXME: currently dequeues are asserted to not fail due to lack of memory.
		_high_prio_job_queue 	= dequeue_new();
		_default_prio_job_queue = dequeue_new();
		_low_prio_job_queue		= dequeue_new();
		if(pthread_create(&thread, NULL, _workqueue_worker, NULL) != 0) {
			_free_workqueues();
			ret = ENOMEM;
			_wq_configured = 0;
		}
		else {
			_wq_configured = 1;
			ret = 0;
		}
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
	if(workqp != NULL) {
		if(attrp != NULL) {
			attr = *attrp;
		}
		else {
			attr = _default_workqueue_attributes;
		}
		workqp->sig = PTHREAD_WORKQUEUE_T_SIG;
		workqp->attr = attr;
		ret = 0;
	}
	else {
		ret = EINVAL;
	}
	return ret;
}

int pthread_workqueue_additem_np(pthread_workqueue_t workq, void *(*workitem_func)(void *), void * workitem_arg, pthread_workitem_handle_t * itemhandlep, unsigned int *gencountp) {
	if(_is_valid_workqueue(&workq)) {
		pthread_workitem_handle_t *new_job = calloc(1, sizeof(pthread_workitem_handle_t));
		if(new_job == NULL) {
			return ENOMEM;
		}
		else {
			new_job->workq = workq;
			new_job->func = workitem_func;
			new_job->arg = workitem_arg;
			dequeue_t *queue = _queue_for_priority(workq.attr.priority);
			pthread_mutex_lock(&_job_queue_mutex);
			dequeue_append(queue, new_job); //check for success/failure (currently no such checks)
			psem_up(&_job_semaphore);
			pthread_mutex_unlock(&_job_queue_mutex);
			*itemhandlep 	= *new_job; // FIXME: this is just a hack to feel like the spec.
			*gencountp 		= 0; // FIXME: what the hell is this parameter anyway? Going to have to read the implimentations.
		}
	}
	else{
		return EINVAL;
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
