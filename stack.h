#ifndef __STACK_H_
#define __STACK_H_

typedef struct stck stack_t;

stack_t *stack_create();
void *stack_pop(stack_t *stck);
void stack_push(stack_t *stck, void *elem);
int stack_size(stack_t *stck);
void stack_destroy(stack_t *stck);

#endif
