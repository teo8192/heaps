#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int cmpfunc(int *a, int *b)
{
	return *a - *b;
}

typedef struct array {
	int *data;
	int num_elements, current;
} array_t;

int *push(array_t *arr, int num)
{
	if (arr->current >= arr->num_elements) {
		arr->num_elements <<= 1;
		arr = realloc(arr, arr->num_elements * sizeof(int));
	}
	arr->data[arr->current++] = num;
	return &arr->data[arr->current - 1];
}

int pop(array_t *arr)
{
	if (arr->current)
		return arr->data[--arr->current];

	return -1;
}

int main(int argc, char **argv)
{
	int min = 0;
	if (argc > 1) {
		if (!strcmp(argv[1], "min")) {
			min = 1;
		} else if (!strcmp(argv[1], "max")) {
			min = 0;
		} else {
			printf("'%s' is unknown\n", argv[1]);
		}
	}
	array_t array;
	array.data = calloc(32, sizeof(int));
	array.num_elements = 32;
	array.current = 0;

	srand(time(NULL));
	heap_t *heap = heap_new((int(*)(void*,void*)) cmpfunc, min);

	int running = 1;
	char input[250];
	int inpt;

	while (running) {
		scanf("%s", input);
		if (!strcmp(input, "exit")) {
			running = 0;
			break;
		}
		if (!strcmp(input, "get")) {
			int *val = heap_del(heap);
			if (val)
				printf("got: %d\n", *(int*) val);
			else
				printf("got null\n");
		} else if (!strcmp(input, "add")) {
			scanf(" %d", &inpt);
			void *val = push(&array, inpt);
			heap_insert(heap, val, val);
		}
		heap_plot(heap);
		scanf("\n");
	}

	free(array.data);
	heap_destroy(heap);
}
