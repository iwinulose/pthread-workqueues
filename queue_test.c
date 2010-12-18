/* Copyright (c) 2010 Charles Duyk and John Stumpo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the names of the authors nor the names of other contributors
 *        may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdincludes.h"
#include "dequeue.h"

#define NUM_ITEMS 50

int main(void) {
	dequeue_t 	*dequeue 	= dequeue_new();
	uint32_t 	size 		= 0;
	int 		*val 		= NULL;
	list_node_t	*node		= NULL;
	//test autoboxing calls
	printf("regular calls (autoboxing pointers)");
	for(int i = 0; i < NUM_ITEMS; i++) {
		val = malloc(sizeof(int));
		*val = i;
		(i % 2) ? dequeue_append(dequeue, val) : dequeue_push(dequeue, val);
	}
	while((size = dequeue_size(dequeue)) > 0) {
		val = ((size % 2) ? dequeue_pop(dequeue) : dequeue_pop_tail(dequeue));
		printf("size: %u val = %d\n", size, *val);
		free(val);
	}
	val = dequeue_pop(dequeue);
	printf("empty queue pop val = %p, size = %u\n", (void *) val, dequeue_size(dequeue));
	val = dequeue_pop_tail(dequeue);
	printf("empty queue pop tail val = %p, size = %u \n", (void *) val, dequeue_size(dequeue));
	
	//two stage allocation
	printf("testing two stage allocation:\n");
	for(int i = 0; i < 50; i++) {
		val = malloc(sizeof(int));
		list_node_t *node = dequeue_node();
		*val = i;
		(i % 2) ? dequeue_push_node(dequeue, node, val) : dequeue_append_node(dequeue, node, val);
	}
	while((size = dequeue_size(dequeue)) > 0) {
		val = ((size % 2) ? dequeue_pop_tail_node(dequeue, &node) : dequeue_pop_node(dequeue, &node));
		printf("size = %u val = %d\n", size, *val);
		free(val);
		dequeue_node_free(node);
	}
	dequeue_free(dequeue);
	return EXIT_SUCCESS;
}
