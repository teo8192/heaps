#ifndef __HEAP_H_
#define  __HEAP_H_

typedef struct heap heap_t;
typedef int (*cmpfunc_t)(void*,void*);

heap_t *heap_new(cmpfunc_t cmpfunc, int min);
void heap_insert(heap_t *heap, void *pri, void *elem);
void heap_destroy(heap_t *heap);
void *heap_del(heap_t *heap);

void heap_plot(heap_t *heap);

#endif
