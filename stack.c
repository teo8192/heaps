#include <stdlib.h>
#include "stack.h"

typedef struct node {
	struct node *prev;
	void *elem;
} node_t;

struct stck {
	node_t *last;
	int size;
};

stack_t *stack_create()
{
	stack_t *stck = calloc(1, sizeof(stack_t));
	if (!stck)
		return NULL;

	return stck;
}

void *stack_pop(stack_t *stck)
{
	if (stck->last) {
		node_t *last = stck->last;
		void *elem = last->elem;
		stck->last = last->prev;
		free(last);
		--stck->size;
		return elem;
	}
	return NULL;
}

void stack_push(stack_t *stck, void *elem)
{
	node_t *new = malloc(sizeof(node_t));
	if (!new)
		return;

	new->elem = elem;
	new->prev = stck->last;
	++stck->size;
	stck->last = new;
}

int stack_size(stack_t *stck)
{
	return stck->size;
}

void stack_destroy(stack_t *stck)
{
	if (stck) {
		node_t *tmp, *current = stck->last;
		while (current) {
			tmp = current->prev;
			free(current);
			current = tmp;
		}
		stck->size = 0;
		stck->last = NULL;
		free(stck);
	}
}

