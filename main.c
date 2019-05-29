#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int cmpfunc(int *a, int *b)
{
	return *a - *b;
}

int main()
{
	int len = 10;
	int data[len];
	srand(time(NULL));
	heap_t *heap = heap_new((int(*)(void*,void*)) cmpfunc, 0);

	for (int i = 0; i < len; ++i) {
		data[i] = rand() % len;
		heap_insert(heap, &data[i], &data[i]);
	}
	heap_plot(heap);
	getchar();
	for (int i = 0; i < len - 1; ++i) {
		printf("%d\n", *(int*)heap_del(heap));
		heap_plot(heap);
		getchar();
	}

	heap_destroy(heap);
}
