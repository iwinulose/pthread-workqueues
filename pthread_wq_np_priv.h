#ifndef PTHREAD_WQ_NP_PRIV_H
#define PTHREAD_WQ_NP_PRIV_H
#ifndef PTHREAD_WQ_NP_H
#error "Please include pthread_wq_np.h"
#else
struct pthread_workqueue_s {
	int lols;
};

struct pthread_workqueue_attr_s {
	int overcommit;
	int priority;
};

extern const pthread_workqueue_attr_t _default_workqueue_attributes;

#endif /* PTHREAD_WQ_NP_H */
#endif /* PTHREAD_WQ_NP_PRIV_H */