#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include "pthread_wq_np.h"
#include "pthread_wq_np_priv.h"


#define IS_VALID_QUEUE_PRIORITY(x) (((x) == WORKQ_HIGH_PRIOQUEUE) || ((x) == WORKQ_DEFAULT_PRIOQUEUE) || ((x) == WORKQ_LOW_PRIOQUEUE))

const pthread_workqueue_attr_t _default_workqueue_attributes = {
	.priority 	= WORKQ_DEFAULT_PRIOQUEUE,
	.overcommit = 0,
};


int pthread_workqueue_attr_init_np(pthread_workqueue_attr_t *attr) {
	int ret = -1;
	if(attr == NULL) {
		ret = EINVAL;
	}
	else {
		attr->priority = WORKQ_DEFAULT_PRIOQUEUE;
		ret = 0;
	}
	return ret;
}

int pthread_workqueue_attr_destroy_np(pthread_workqueue_attr_t *attr) {
	return 0;
}

int pthread_workqueue_attr_getovercommit_np(pthread_workqueue_attr_t *attr, int *ocommp) {
	return 0;
}

int pthread_workqueue_attr_setovercommit_np(pthread_workqueue_attr_t *attr, int ocomm) {
	return 0;
}

int pthread_workqueue_attr_getqueuepriority_np(pthread_workqueue_attr_t *attr, int *qpriop) {
	int ret = -1;
	if (attr == NULL || qpriop == NULL) {
		ret = EINVAL;
	}
	else {
		*qpriop = attr->priority;
	}
	return ret;
}

int pthread_workqueue_attr_setqueuepriority_np(pthread_workqueue_attr_t *attr, int qprio) {
	int ret = -1;
	
	if(IS_VALID_QUEUE_PRIORITY(qprio)) {
		attr->priority = qprio;
		ret = 0;
	}
	else {
		ret = EINVAL;
	}
	return ret;
}