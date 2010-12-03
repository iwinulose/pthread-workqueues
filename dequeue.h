//Charles Duyk
//charles.duyk@gmail.com
//John Stumpo
//stump@jstump.com
#ifndef DEQUEUE_H 
#define DEQUEUE_H

struct list_node_s;
typedef struct list_node_s list_node_t;
struct list_node_s {
	list_node_t *next;
	list_node_t *prev;
	void *data;
};

typedef struct dequeue_s {
	list_node_t *head;
	list_node_t *tail;
	size_t size;
} dequeue_t;


dequeue_t * dequeue_new(void);
void 		dequeue_free(dequeue_t *);
size_t		dequeue_size(dequeue_t *);
void *		dequeue_head(dequeue_t *);
void *		dequeue_tail(dequeue_t *);
void *		dequeue_pop(dequeue_t *);
void *		dequeue_pop_tail(dequeue_t *);
void		dequeue_push(dequeue_t *, void *);
void		dequeue_append(dequeue_t *, void *);

#endif /*DEQUEUE_H*/
