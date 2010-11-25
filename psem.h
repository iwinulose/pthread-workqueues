//Portable counting semaphore
typedef struct psem_s {
	pthread_mutex_t lock;
	pthread_cond_t condvar;
	int counter;
} psem_t;

void sem_init(psem_t *, int);
psem_t *sem_new(int);
void sem_free(psem_t *);
void sem_up(psem_t *);
void sem_down(psem_t *);

