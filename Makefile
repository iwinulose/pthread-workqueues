CC=gcc 
CFLAGS=-Wall -Wextra -pedantic -std=c99 -g -O0
LDFLAGS= -lpthread
OBJECTS= sem_test.o psem.o pthread_wq_np.o pthread_wq_attr_np.o
LIBS= libpthread_wq_np.so
EXECUTABLES=sem_test

.PHONY: all
all: $(OBJECTS) $(LIBS) $(EXECUTABLES)

.PHONY: lib
lib: libraries

.PHONY: libraries
libraries: $(LIBS)

test: sem_test
sem_test: psem.o sem_test.o
sem_test.o: sem_test.c

psem.o: psem.c psem.h

pthread_wq_np.o: pthread_wq_np.c pthread_wq_np.h pthread_wq_np_priv.h

pthread_wq_attr_np.o: pthread_wq_attr_np.c pthread_wq_np.h pthread_wq_np_priv.h

libpthread_wq_np.so: pthread_wq_np.o pthread_wq_attr_np.o
	$(CC) -shared -fPIC -o $@ $^

.PHONY: clean
clean:
	rm -rf *.dSYM $(OBJECTS) $(LIBS) $(EXECUTABLES) 
