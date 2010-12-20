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

#ifndef PTHREAD_WQ_NP_H
#define PTHREAD_WQ_NP_H

/* Work queue priority attributes. */
#define	WORKQ_HIGH_PRIOQUEUE	0	/* Assign to high priority queue. */
#define WORKQ_DEFAULT_PRIOQUEUE	1	/* Assign to default priority queue. */
#define WORKQ_LOW_PRIOQUEUE		2	/* Assign to low priority queue. */

typedef void* pthread_workqueue_t;
typedef struct pthread_workqueue_attr_s pthread_workqueue_attr_t; 
typedef struct pthread_workitem_handle_s pthread_workitem_handle_t;

struct pthread_workqueue_attr_s {
	unsigned int sig;
	int overcommit;
	int priority;
};

struct pthread_workitem_handle_s {
	pthread_workqueue_t workq;
	void *(*func)(void *);
	void *arg;
};

/*
	Initializes the workqueue system. Need only be called once per application. After the first successful invocation, subsequent invocations have no effect and will always succeed.
	
	Returns
		0: 		Success
		ENOMEM:	There were insufficient resources to initialize the workqueue system.
*/
int pthread_workqueue_init_np(void);

/*
	Creates a new work queue with the attributes specified by attr. If attr is NULL, the default attributes are used. The new work queue's handle is returned in workqp. If an invocation of pthread_workqueeu_create_np fails, the value which results in workqp is unspecified.
	
	Returns
		0:		Success
		ENOMEM:	There was insufficent memory to create a new workqueue
*/
int pthread_workqueue_create_np(pthread_workqueue_t *workqp, const pthread_workqueue_attr_t * attr);

/*
	Enqueues workitem_func on the workqueue specified by workq. Its argument is specified in workitem_arg. A handle is returned in itemhandlep. I have no idea what gencountp is used for.
	
	Returns
		0:		Success
		ENOMEM:	There was insufficient memory to add the item to the workqueue
		EINVAL:	Invalid workqueue handle
		ESRCH:	Couldn't find a workqueue
*/
int pthread_workqueue_additem_np(pthread_workqueue_t workq, void *(*workitem_func)(void *), void * workitem_arg, pthread_workitem_handle_t * itemhandlep, unsigned int *gencountp);

/*
	Initializes the pthread_workqueue_attr_t with the default attributes.
	
	ENOMEM:	There was insufficient memory
*/
int pthread_workqueue_attr_init_np(pthread_workqueue_attr_t *attr);

/*
	
*/
int pthread_workqueue_attr_destroy_np(pthread_workqueue_attr_t *attr);

/*

*/ 
int pthread_workqueue_attr_getovercommit_np(pthread_workqueue_attr_t *attr, int *ocommp);

/*

*/
int pthread_workqueue_attr_setovercommit_np(pthread_workqueue_attr_t *attr, int ocomm);

/*

*/
int pthread_workqueue_attr_getqueuepriority_np(pthread_workqueue_attr_t *attr, int *qpriop);

/*

*/
int pthread_workqueue_attr_setqueuepriority_np(pthread_workqueue_attr_t *attr, int qprio);

#endif /* PTHREAD_WQ_NP_H */
