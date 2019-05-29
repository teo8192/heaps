#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "plot.h"

typedef struct node {
	struct node *left, *right;
	void *pri, *elem;
} node_t;

struct heap {
	node_t *root;
	int size;
	int min;
	cmpfunc_t cmpfunc;
};

heap_t *heap_new(cmpfunc_t cmpfunc, int min)
{
	heap_t *heap = malloc(sizeof(heap_t));
	if (!heap)
		return NULL;
	heap->root = NULL;
	heap->size = 0;
	heap->cmpfunc = cmpfunc;
	heap->min = min;
	return heap;
}

node_t *node_new(void *pri, void *elem)
{
	node_t *new = malloc(sizeof(node_t));
	if (!new)
		return NULL;
	new->elem = elem;
	new->pri = pri;

	new->left = NULL;
	new->right = NULL;

	return new;
}

static node_t *_insert(heap_t *heap, node_t *cur, node_t *new, int n, int l)
{
	if (!cur)
		return new;

	l >>= 1;
	if (!(l & n)) {
		if ((heap->cmpfunc(new->pri, cur->pri) < 0) ==  heap->min) {
			new->left = cur->left;
			cur->left = NULL;
			new->right = cur->right;
			cur->right = NULL;
			new->left = _insert(heap, new->left, cur, n, l);
			return new;
		}

		cur->left = _insert(heap, cur->left, new, n, l);
	} else {
		if ((heap->cmpfunc(new->pri, cur->pri) < 0) ==  heap->min) {
			new->right = cur->right;
			cur->right = NULL;
			new->left = cur->left;
			cur->left = NULL;
			new->right = _insert(heap, new->right, cur, n, l);
			return new;
		}

		cur->right = _insert(heap, cur->right, new, n, l);
	}
	return cur;
}

static void convert(int *n, int *lev)
{
	if (*n >= *lev) {
		*n -= *lev;
		*lev <<= 1;
		convert(n, lev);
	}
}

void heap_insert(heap_t *heap, void *pri, void *elem)
{
	node_t *new = node_new(pri, elem);
	int n = heap->size - 1;
	int level = 2;
	convert(&n, &level);
	heap->root = _insert(heap, heap->root, new, n, level);
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
		_destroy_nodes(heap->root);
		free(heap);
	}
}

static node_t *get_node(heap_t *heap, node_t *root)
{
	if (!root->left)
		return NULL;
	if (!root->right)
		return root->left;
	if ((heap->cmpfunc(root->left->pri, root->right->pri) < 0) == heap->min)
		return root->left;

	return root->right;
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

static node_t *_fix(heap_t *heap, node_t *node, int n, int l)
{
	l >>= 1;
	//_insert();
	if (!(n & l)) {
		//left
		if (node->left) {
			node_t *ret = _fix(heap, node->left, n, l);
			if (ret == node->left)
				node->left = NULL;

			return ret;
		}

		return node;
	} else {
		if (node->right) {
			node_t *ret = _fix(heap, node->right, n, l);
			if (ret == node->right)
				node->right = NULL;

			return ret;
		}

		return node;
	}
}

static void _sift_down(heap_t *heap, node_t *node)
{
	if (!node || !node->left)
		return;
	if (!node->right) {
		if ((heap->cmpfunc(node->pri, node->left->pri) > 0) == heap->min) {
			swap_nodes(node->left, node);
#ifdef DEBUG
			heap_plot(heap);
			getchar();
#endif
		}
	} else if ((heap->cmpfunc(node->left->pri, node->right->pri) < 0) == heap->min) {
		// left
		if ((heap->cmpfunc(node->pri, node->left->pri) > 0) == heap->min) {
			swap_nodes(node->left, node);
#ifdef DEBUG
			heap_plot(heap);
			getchar();
#endif
			_sift_down(heap, node->left);
		}
	} else {
		// right
		if ((heap->cmpfunc(node->pri, node->right->pri) > 0) == heap->min) {
			swap_nodes(node->right, node);
#ifdef DEBUG
			heap_plot(heap);
			getchar();
#endif
			_sift_down(heap, node->right);
		}
	}
}

static node_t *fix(heap_t *heap, node_t *node)
{
	int n = heap->size - 2;
	int level = 2;
	convert(&n, &level);
	node_t *last_insert = _fix(heap, node, n, level);

	//printf("%d\n", *(int*)last_insert->pri);

	swap_nodes(node, last_insert);

#ifdef DEBUG
	heap_plot(heap);
	getchar();
#endif
	_sift_down(heap, heap->root);
	return last_insert;
}

void *heap_del(heap_t *heap)
{
	if (heap->root) {
		void *elem = heap->root->elem;

		node_t *oldroot = fix(heap, heap->root);
		--heap->size;

		free(oldroot);
		//printf("%p\n", heap->root);
		return elem;
	}
	return NULL;
}

static char *strnode(char *buf, node_t *node)
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
	sprintf(buf, "%d", *(int*) node->pri);
	return buf;
}

static void _heap_plot(plot_t *plot, node_t *node)
{
	char from[250], to[250];

	if (!node || node == (void*) 0x100)
		return;

	strnode(from, node);

	if (node->left)
		plot_addlink2(plot, node, node->left, from, strnode(to, node->left));
	if (node->right)
		plot_addlink2(plot, node, node->right, from, strnode(to, node->right));

	_heap_plot(plot, node->left);
	_heap_plot(plot, node->right);
}

void heap_plot(heap_t *heap)
{
	plot_t *plot = plot_create("heap");
	if (!plot)
		return;

	_heap_plot(plot, heap->root);

	plot_doplot(plot);
}
