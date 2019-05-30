#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"
#include "plot.h"
#include "stack.h"

typedef struct node {
	struct node *left, *right, *parent;
	void *pri, *elem;
	unsigned int goright;
} node_t;

struct heap {
	node_t *root;
	int size;
	int min;
	node_t *last_inserted;
	stack_t *inserted;
	cmpfunc_t cmpfunc;
};

#define GORIGHT(x) ((x) & (1 << 2))
#define NUMCHILD(x) ((x) & ((1 << 2) - 1))

heap_t *heap_new(cmpfunc_t cmpfunc, int min)
{
	heap_t *heap = malloc(sizeof(heap_t));
	if (!heap)
		return NULL;
	heap->root = NULL;
	heap->last_inserted = NULL;
	heap->size = 0;
	heap->cmpfunc = cmpfunc;
	heap->min = min;

	heap->inserted = stack_create();

	return heap;
}

static node_t *node_new(void *pri, void *elem)
{
	node_t *new = malloc(sizeof(node_t));
	if (!new)
		return NULL;
	new->elem = elem;
	new->pri = pri;

	new->left = NULL;
	new->right = NULL;
	new->parent = NULL;
	new->goright = 0;

	return new;
}

static void swap_nodes(node_t *a, node_t *b)
{
	void *tmp = a->elem;
	a->elem = b->elem;
	b->elem = tmp;

	tmp = a->pri;
	a->pri = b->pri;
	b->pri = tmp;
}

/* log n insertion */
static int _insert(heap_t *heap, node_t *cur, node_t *new)
{
	int retval;
	node_t *inserted;
	if (!cur->left || !cur->right) {
		new->parent = cur;
		inserted = new;
		stack_push(heap->inserted, heap->last_inserted);
		heap->last_inserted = new;
		cur->goright = !cur->left;
		retval = !!cur->left;
		if (cur->left)
			cur->right = new;
		else
			cur->left = new;
	} else if (cur->goright) {
		retval = _insert(heap, cur->right, new);
		inserted = cur->right;

		if (retval)
			cur->goright = 0;
	} else {
		retval = _insert(heap, cur->left, new);
		inserted = cur->left;

		if (retval) {
			cur->goright = 1;
			retval = 0;
		}
	}

	if ((heap->cmpfunc(cur->pri, inserted->pri) > 0) == heap->min)
		swap_nodes(cur, inserted);

	return retval;
}

void heap_insert(heap_t *heap, void *pri, void *elem)
{
	node_t *new = node_new(pri, elem);
	if (!heap->root) {
		heap->root = new;
		heap->last_inserted = new;
	} else
		_insert(heap, heap->root, new);

	++heap->size;
}

static void _destroy_nodes(node_t *node)
{
	if (node) {
		_destroy_nodes(node->left);
		_destroy_nodes(node->right);
		free(node);
	}
}

void heap_destroy(heap_t *heap)
{
	if (heap) {
		stack_destroy(heap->inserted);
		_destroy_nodes(heap->root);
		// just clear the heap in case they 
		// try to use it for shit afterwards
		memset(heap, 0, sizeof(heap_t));
		free(heap);
	}
}

static void _sift_down(heap_t *heap, node_t *node)
{
	if (!node || !node->left)
		return;
	if (!node->right) {
		if ((heap->cmpfunc(node->pri, node->left->pri) > 0) == heap->min) {
			swap_nodes(node->left, node);
		}
	} else if ((heap->cmpfunc(node->left->pri, node->right->pri) < 0) == heap->min) {
		// left
		if ((heap->cmpfunc(node->pri, node->left->pri) > 0) == heap->min) {
			swap_nodes(node->left, node);
			_sift_down(heap, node->left);
		}
	} else {
		// right
		if ((heap->cmpfunc(node->pri, node->right->pri) > 0) == heap->min) {
			swap_nodes(node->right, node);
			_sift_down(heap, node->right);
		}
	}
}

/* log n deletion, (fix up and sift down) */
void *heap_del(heap_t *heap)
{
	if (heap->root) {
		void *elem = heap->root->elem;
		node_t *remnode = heap->last_inserted;

		heap->last_inserted = stack_pop(heap->inserted);
		
		if (remnode->parent) {
			if (remnode == remnode->parent->left) {
				// left child
				remnode->parent->goright = 0;
				remnode->parent->left = NULL;
			} else {
				// right child
				node_t *tmp = remnode;
				while (tmp->parent) {
					tmp->parent->goright = (tmp == tmp->parent->right);
					tmp = tmp->parent;
				}

				remnode->parent->right = NULL;
			}
			
			swap_nodes(heap->root, remnode);
			_sift_down(heap, heap->root);
			--heap->size;
		} else {
			heap->size = 0;
			heap->root = NULL;
			heap->last_inserted = NULL;
		}

		free(remnode);
		return elem;
	}
	return NULL;
}

static char *strnode(char *buf, node_t *node, heap_t *heap)
{
	if (!node || node == (void*)0x100) {
		sprintf(buf, "NULL");
		return buf;
	}
	int n = -1;
	if (!node->pri)
		node->pri = &n;
	if (!node->elem)
		node->elem = &n;
	sprintf(buf, "%d-%s%s", *(int*) node->pri, node->goright ? ">" : "<",
			node == heap->root ? " R" : node == heap->last_inserted ? " L" : "");
	return buf;
}

static void _heap_plot(plot_t *plot, node_t *node, heap_t *heap)
{
	char from[250], to[250];

	if (!node || node == (void*) 0x100)
		return;

	strnode(from, node, heap);

	if (node->left)
		plot_addlink2(plot, node, node->left, from, strnode(to, node->left, heap));
	if (node->right)
		plot_addlink2(plot, node, node->right, from, strnode(to, node->right, heap));

	_heap_plot(plot, node->left, heap);
	_heap_plot(plot, node->right, heap);
}

void heap_plot(heap_t *heap)
{
	plot_t *plot = plot_create("heap");
	if (!plot)
		return;

	_heap_plot(plot, heap->root, heap);

	plot_doplot(plot);
}
