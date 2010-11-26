//Portable counting semaphore
typedef struct psem_s {
	pthread_mutex_t lock;
	pthread_cond_t condvar;
	int counter;
} psem_t;

void psem_init(psem_t *, int);
psem_t *psem_new(int);
void psem_free(psem_t *);
void psem_up(psem_t *);
void psem_down(psem_t *);

