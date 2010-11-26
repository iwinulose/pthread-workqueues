CC=gcc 
CFLAGS=-Wall -Wextra -pedantic -std=c99 -D_REENTRANT -g -O0
LDFLAGS= -lpthread
OBJECTS= sem_test.o queue_test.o wq_test.o dequeue.o psem.o pthread_wq_np.o pthread_wq_attr_np.o
LIBS= libpthread_wq_np.so
TESTS=sem_test queue_test wq_test
EXECUTABLES=$(TESTS)

.PHONY: all
all: $(OBJECTS) $(LIBS) $(EXECUTABLES)

.PHONY: lib
lib: $(LIBS)

.PHONY: test
test: $(TESTS) 
sem_test: psem.o sem_test.o
sem_test.o: sem_test.c
queue_test: queue_test.o dequeue.o
queue_test.o: queue_test.c
wq_test: LDFLAGS += -L. -ldl -lpthread_wq_np
wq_test: wq_test.o 
wq_test.o: wq_test.c

psem.o: CFLAGS += -fvisibility=hidden
psem.o: psem.c psem.h
dequeue.o: CFLAGS += -fvisibility=hidden
dequeue.o: dequeue.c dequeue.h

pthread_wq_np.o: psem.o dequeue.o pthread_wq_np.c pthread_wq_np.h pthread_wq_np_priv.h
pthread_wq_attr_np.o: pthread_wq_attr_np.c pthread_wq_np.h pthread_wq_np_priv.h
libpthread_wq_np.so: pthread_wq_np.o pthread_wq_attr_np.o dequeue.o psem.o
	$(CC) -shared -fPIC -o $@ $^

.PHONY: clean
clean:
	rm -rf *.dSYM *.o *.so $(EXECUTABLES) 
