//Charles Duyk
//charles.duyk@gmail.com
//John Stumpo
//stump@jstump.com 

#ifndef PTHREAD_WQ_NP_H
#define PTHREAD_WQ_NP_H

/* Work queue priority attributes. */
#define	WORKQ_HIGH_PRIOQUEUE	0	/* Assign to high priority queue. */
#define WORKQ_DEFAULT_PRIOQUEUE	2	/* Assign to default priority queue. */
#define WORKQ_LOW_PRIOQUEUE		4	/* Assign to low priority queue. */


typedef struct pthread_workqueue_s pthread_workqueue_t;
typedef struct pthread_workqueue_attr_s pthread_workqueue_attr_t; 
typedef struct pthread_workitem_handle_s pthread_workitem_handle_t;

struct pthread_workqueue_attr_s {
	unsigned int sig;
	int overcommit;
	int priority;
};

struct pthread_workqueue_s {
	unsigned int sig;
	pthread_workqueue_attr_t attr;
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
