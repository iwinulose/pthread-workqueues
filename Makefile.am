noinst_PROGRAMS = wq_test queue_test sem_test
sem_test_SOURCES = psem.c sem_test.c
sem_test_CFLAGS = $(AM_CFLAGS)
queue_test_SOURCES = queue_test.c dequeue.c
queue_test_CFLAGS = $(AM_CFLAGS)
wq_test_SOURCES = wq_test.c
wq_test_CFLAGS = $(AM_CFLAGS)
wq_test_LDADD = libpthread_wq_np.la -ldl

lib_LTLIBRARIES = libpthread_wq_np.la
libpthread_wq_np_la_SOURCES = psem.c dequeue.c pthread_wq_np.c pthread_wq_np.h pthread_wq_np_priv.h

include_HEADERS = pthread_wq_np.h
