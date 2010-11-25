#include "stdincludes.h"
#include "dequeue.h"


int main(void) {
	dequeue_t *dequeue = dequeue_new();
	for(int i = 0; i < 50; i++) {
		int *lols = malloc(sizeof(int));
		*lols = i;
		(i % 2) ? dequeue_append(dequeue, lols) : dequeue_push(dequeue, lols);
	}
	uint32_t size;
	int *val;
	while((size = dequeue_size(dequeue)) > 0) {
		val = (size % 2 ? dequeue_pop(dequeue) : dequeue_pop_tail(dequeue));
		printf("size: %u val = %d\n", size, *val);
		free(val);
	}
	dequeue_free(dequeue);
	return EXIT_SUCCESS;
}
