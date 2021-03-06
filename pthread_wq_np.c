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
static dequeue_t * _job_queues[NUM_JOB_QUEUES];
static pthread_mutex_t _job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static psem_t _job_semaphore;
static int num_workers = 0;
#define IS_VALID_QUEUE_PRIORITY(x) (((x) == WORKQ_HIGH_PRIOQUEUE) \
										|| ((x) == WORKQ_DEFAULT_PRIOQUEUE) \
										|| ((x) == WORKQ_LOW_PRIOQUEUE))

static const pthread_workqueue_attr_t _default_workqueue_attributes = {
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

static int _is_valid_workqueue(wq_t *wq) {
	int ret = 0;
	if(wq == NULL) {
		ret = 0;
	}
	else if(wq->sig != PTHREAD_WORKQUEUE_T_SIG) {
		ret = 0;
	}
	else if(!_is_valid_attr(&wq->attr)) {
		ret = 0;
	}
	else{
		ret = 1;
	}
	return ret;
}

static dequeue_t * _queue_for_priority(int priority) {
	return IS_VALID_QUEUE_PRIORITY(priority) ? _job_queues[priority] : NULL;
}

static void * _workqueue_worker(void *arg) {
	(void) arg;
	int running = 1;
	while(running) {
		psem_down(&_job_semaphore);
		pthread_workitem_handle_t *job = NULL;
		list_node_t *node = NULL;
		pthread_mutex_lock(&_job_queue_mutex);
		for(int i = 0; i < NUM_JOB_QUEUES; i++) {
			job = dequeue_pop_node(_job_queues[i], &node);
			if(job != NULL) {
				break;
			}
		}
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

static int _spawn_worker(void) {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, _workqueue_worker, NULL);
	if (ret == 0) {
		num_workers++;
	}
	return ret;
}

static void _free_job_queues(void) {
	dequeue_t * queue = NULL;
	for(int i = 0; i < NUM_JOB_QUEUES; i++) {
		queue = _job_queues[i];
		if(queue != NULL) {
			dequeue_free(queue);
		}
	}
}

//Initializes the job queues. Returns 0 on success, nonzero otherwise. If _init_job_queues fails for any reason, it shall be as if it had never been called.
static int _init_job_queues(void) {
	//FIXME: currently dequeues are asserted to not fail due to lack of memory.
	if(_wq_configured) {
		return 1;
	}
	for(int i = 0; i < NUM_JOB_QUEUES; i++) {
		if((_job_queues[i] = dequeue_new()) == NULL) {
			_free_job_queues();
			return 1;
		}
	}
	return 0;
}

int pthread_workqueue_init_np(void) {
	pthread_mutex_lock(&_init_mutex);
	if(!_wq_configured) {
		psem_init(&_job_semaphore, 0);
		if(_init_job_queues() != 0) {
			goto out_bad;
		}
		_wq_configured = 1;
	}
	pthread_mutex_unlock(&_init_mutex);
	return 0;
out_bad:
	_wq_configured = 0;
	pthread_mutex_unlock(&_init_mutex);
	return ENOMEM;
}

int pthread_workqueue_create_np(pthread_workqueue_t *workqp, const pthread_workqueue_attr_t * attrp) {
	if(workqp == NULL) {
		return EINVAL;
	}
	pthread_workqueue_attr_t attr;
	if(attrp != NULL) {
		if(!_is_valid_attr((pthread_workqueue_attr_t *)attrp)) {
			return EINVAL;
		}
		attr = *attrp;
	}
	else {
		attr = _default_workqueue_attributes;
	}
	wq_t *wq = calloc(1, sizeof(wq_t));
	if(wq == NULL) {
		return ENOMEM;
	}
	wq->sig = PTHREAD_WORKQUEUE_T_SIG;
	wq->attr = attr;
	*workqp = (pthread_workqueue_t) wq;
	return 0;
}

int pthread_workqueue_additem_np(pthread_workqueue_t workq, void *(*workitem_func)(void *), void * workitem_arg, pthread_workitem_handle_t * itemhandlep, unsigned int *gencountp) {
	wq_t *wq = (wq_t *) workq;
	if(_is_valid_workqueue(wq) &&_wq_configured) {
		pthread_workitem_handle_t *new_job = calloc(1, sizeof(pthread_workitem_handle_t));
		if(new_job == NULL) {
			return ENOMEM;
		}
		else {
			//Because the attr was guaranteed to be valid (in _is_valid_workqueue), queue should never be NULL
			dequeue_t *queue = _queue_for_priority(wq->attr.priority);
			assert(queue != NULL);
			new_job->workq = workq;
			new_job->func = workitem_func;
			new_job->arg = workitem_arg;
			list_node_t *node = dequeue_node();
			if(node == NULL) {
				free(new_job);
				return ENOMEM;
			}
			pthread_mutex_lock(&_job_queue_mutex);
			dequeue_append_node(queue, node, new_job); //FIXME: check for success/failure (currently no such checks)
			psem_up(&_job_semaphore);
			pthread_mutex_unlock(&_job_queue_mutex);
			if(psem_peek(&_job_semaphore) >= 0) { //FIXME: we might and likely accidentally will spawn workers unintentinally here
				_spawn_worker();
			}
			if(itemhandlep != NULL) {
				*itemhandlep 	= *new_job; // FIXME: this is just a hack to feel like the spec.
			}
			if(gencountp != NULL) {
				*gencountp 		= 0; // FIXME: In Apple's implimentation this is used to keep track of how many times a given workitem struct has been used.
			}
		}
	}
	else{
		return EINVAL;
	}
	return 0;
}

int pthread_workqueue_attr_init_np(pthread_workqueue_attr_t *attrp) {
	int ret = -1;
	if(attrp == NULL) {
		ret = EINVAL;
	}
	else {
		*attrp = _default_workqueue_attributes;
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_destroy_np(pthread_workqueue_attr_t *attrp) {
	return _is_valid_attr(attrp) ? 0 : EINVAL;
}

int pthread_workqueue_attr_getovercommit_np(pthread_workqueue_attr_t *attrp, int *ocommp) {
	int ret = -1;
	if(!_is_valid_attr(attrp) || ocommp == NULL) {
		ret = EINVAL;
	}
	else {
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_setovercommit_np(pthread_workqueue_attr_t *attrp, int ocomm) {
	int ret = EINVAL;
	if(_is_valid_attr(attrp)) {
		attrp->overcommit = ocomm;
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

int pthread_workqueue_attr_setqueuepriority_np(pthread_workqueue_attr_t *attrp, int qprio) {
	int ret = -1;
	if(_is_valid_attr(attrp) && IS_VALID_QUEUE_PRIORITY(qprio)) {
		attrp->priority = qprio;
		ret = 0;
	}
	else {
		ret = EINVAL;
	}
	return ret;
}
