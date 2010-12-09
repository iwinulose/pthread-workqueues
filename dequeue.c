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
#include "fastpath.h"
#ifndef NO_PRAGMAS
#pragma mark Functional prototypes
#endif
typedef void (*list_node_function)(void *context, list_node_t *);
static void _list_node_apply_function(list_node_t *, list_node_function, void * context, list_node_t *);

#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Freelist declarations & functions
#endif 

//FIXME: figure out what to do about error reporting (can't have a workqueue fail an assert and kill a client application)

static void __free_node(void *context, list_node_t *node) {
	(void) context; //shut up compiler
	list_node_t *next = node->next;
	list_node_t *prev = node->prev;
	if(prev) {
		prev->next = next;
	}
	if(next) {
		next->prev = prev;
	}
	node->data = NULL;
	free(node);
}

static void _free_node(list_node_t *node) {
	__free_node(NULL, node);
}

static void _free_all_nodes(dequeue_t *list) {
	_list_node_apply_function(list->head, __free_node, NULL, NULL);
}

static list_node_t * _get_free_node(void) {
	list_node_t *ret = calloc(1, sizeof(list_node_t));
	assert(ret != NULL);
	return ret;
}

static list_node_t * _package_data(void *data) {
	list_node_t *new_node = _get_free_node();
	new_node->data = data;
	return new_node;
}

#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Apply functions
#endif

static void _list_node_apply_function(list_node_t *start, list_node_function function, void * context, list_node_t *final) {
	list_node_t *node = start;
	list_node_t *next;
	while(node != final && node != NULL) {
		next = node->next;
		function(context, node);
		node = next;
	}
}

#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Accessing nodes
#endif
//returns the node after the given node in teh given list, or NULL if this node is the tail
static list_node_t * _list_node_after(dequeue_t *list, list_node_t *node) {
	list_node_t *ret = node->next;
	return (ret != list->tail) ? ret : NULL; 
}

//returns the node before the given node in the given list, or NULL if this node is the head
static list_node_t * _list_node_before(dequeue_t *list, list_node_t *node) {
	list_node_t *ret = node->prev;
	return (ret != list->head) ? ret : NULL;
}

//returns the first node after the sentinel, or NULL if the list is empty
static list_node_t * _list_first_node(dequeue_t *list) {
	return _list_node_after(list, list->head);
}

//returns the first node after the sentinel, or NULL if the list is empty
static list_node_t * _list_last_node(dequeue_t *list) {
	return _list_node_before(list, list->tail);
}
#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Inserting and removing nodes
#endif
static void _increment_list_size(dequeue_t *list) {
	size_t new_size = list->size + 1;
	list->size = new_size;
}

static void _decrement_list_size(dequeue_t *list) {
	size_t old_size = list->size;
	assert(old_size > 0);
	list->size = old_size - 1;
}

static void _insert_node_before(dequeue_t *list, list_node_t *node, list_node_t *where) {
	assert(where != list->head);
	list_node_t *prev = where->prev;
	where->prev = node;
	node->next = where;
	node->prev = prev;
	prev->next = node;
	_increment_list_size(list);
}

static void _insert_node_after(dequeue_t *list, list_node_t *node, list_node_t *where) {
	assert(where != list->tail);
	list_node_t *next = where->next;
	where->next = node;
	node->prev = where;
	node->next = next;
	next->prev = node;
	_increment_list_size(list);
}

static void * _list_remove_node(dequeue_t *list, list_node_t *node) {
	assert(list->size > 0);
	void * ret;
	ret = node->data;
	_free_node(node);
	_decrement_list_size(list);
	return ret;
}

#ifndef NO_PRAGMAS
#pragma mark -
#pragma mark Creating and destroying lists
#endif

dequeue_t * dequeue_new(void) {
	dequeue_t *list = calloc(1, sizeof(dequeue_t));
	assert(list != NULL);
	list_node_t *head_sentinel = _get_free_node();
	list_node_t *tail_sentinel = _get_free_node();
	head_sentinel->next = tail_sentinel;
	head_sentinel->prev = NULL;
	tail_sentinel->prev = head_sentinel;
	tail_sentinel->next = NULL;
	list->head = head_sentinel;
	list->tail = tail_sentinel;
	return list;
}

void dequeue_free(dequeue_t *list) {
	//this function takes a shortcut--since we're freeing the list anyway, we just free the nodes directly, including the sentinels
	if(exists_fast(list)) {
		_free_all_nodes(list);
		free(list);
	}
}

/*static void _destroy_node(void *context, list_node_t *node) {
	(void) context;
	free(node);
}*/

//List utilities

size_t dequeue_size(dequeue_t *list) {
	return list->size;
}

//accessing data
//returns the data associated with the first node in the list, or NULL if the list is empty
void * dequeue_head(dequeue_t *list) {
	void * ret = NULL;
	if(exists_fast(list)) {
		if(list->size > 0) {
			list_node_t *node = _list_first_node(list);
			ret = node->data;
		}
	}
	return ret;
}

//returns the data associated with the last node in the list, or NULL if the list is empty
void * dequeue_tail(dequeue_t *list) {
	void * ret = NULL;
	if(exists_fast(list)) {
		if(list->size != 0) {
			list_node_t *node = _list_last_node(list);
			ret = node->data;
		}
	}
	return ret;
}

//returns the data associated with the first node of the list and removes that node from the list. Returns NULL if the list was previously empty.
void * dequeue_pop(dequeue_t *list) { 
	//there is some code repitition here, but I think this is cleanest way to express this.  
	void * ret = NULL;
	if(exists_fast(list)) {
		list_node_t *node = _list_first_node(list);
		if(exists(node)) {
			ret = _list_remove_node(list, node);
		}
	}
	return ret;
}

void * dequeue_pop_tail(dequeue_t *list) {
	void * ret = NULL;
	if(exists_fast(list)) {
		list_node_t *node = _list_last_node(list);
		if(exists(node)) {
			ret = _list_remove_node(list, node);
		}
	}
	return ret;
}

//places this data in a new node at the front of the list.
void dequeue_push(dequeue_t *list, void *data) {
	if(exists_fast(list)) {
		list_node_t *node = _package_data(data);
		_insert_node_after(list, node, list->head);
	}
}

//places this data in a new node at the end of the list
void dequeue_append(dequeue_t *list, void *data) {
	if(exists_fast(list)) {
		list_node_t *node = _package_data(data);
		_insert_node_before(list, node, list->tail);
	}
}

//Here's the hack to work around the lack of lambdas:
//static void list_apply_operator(dequeue_t *list, list_operator_f operator) {
//	list->operator = operator;
//	_list_node_apply_function(_list_first_node(list), _perform_list_operator, list->tail);
//}
