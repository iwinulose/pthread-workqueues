//Portable counting semaphore

typedef struct psem_s psem_t;


void sem_init(psem_t *);
psem_t *sem_new(void);
void sem_free(psem_t *);
void sem_up(psem_t *);
void sem_down(psem_t *);
