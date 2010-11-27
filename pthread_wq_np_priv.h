#ifndef PTHREAD_WQ_NP_PRIV_H
#define PTHREAD_WQ_NP_PRIV_H
#ifndef PTHREAD_WQ_NP_H
#error "Please include pthread_wq_np.h"
#else

typedef void *(*workitem_f)(void*);



#define PTHREAD_WORKQUEUE_T_SIG 0xffefa231
#define PTHREAD_WORKQUEUE_ATTR_T_SIG 0xef0c1f2f

#endif /* PTHREAD_WQ_NP_H */
#endif /* PTHREAD_WQ_NP_PRIV_H */
