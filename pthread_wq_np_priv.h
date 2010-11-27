#ifndef PTHREAD_WQ_NP_PRIV_H
#define PTHREAD_WQ_NP_PRIV_H
#ifndef PTHREAD_WQ_NP_H
#error "Please include pthread_wq_np.h"
#else

#define PTHREAD_WORKQUEUE_T_SIG 0xffefa231
#define PTHREAD_WORKQUEUE_ATTR_T_SIG 0xef0c1f2f

struct pthread_workqueue_s {
	unsigned int sig;
	int lols;
};

struct pthread_workqueue_attr_s {
	unsigned int sig;
	int overcommit;
	int priority;
};

extern const pthread_workqueue_attr_t _default_workqueue_attributes;

#endif /* PTHREAD_WQ_NP_H */
#endif /* PTHREAD_WQ_NP_PRIV_H */
