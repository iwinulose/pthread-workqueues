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
int			dequeue_push(dequeue_t *, void *);
int			dequeue_append(dequeue_t *, void *);

//two stage allocation
list_node_t * dequeue_node(void);
void		dequeue_node_free(list_node_t *);
int			dequeue_push_node(dequeue_t *, list_node_t *, void *);
int 		dequeue_append_node(dequeue_t *, list_node_t *, void *);
void *		dequeue_pop_node(dequeue_t *, list_node_t **);
void *		dequeue_pop_tail_node(dequeue_t *, list_node_t **);
#endif /*DEQUEUE_H*/
