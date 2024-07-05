#include "xre_memory.h"
#include "xre_operations.h"
#include "xre_utils.h"
#include "xre_log.h"
#include "array.h"
#include "xre_xdp.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

array_t *g_stack = NULL;
size_t g_stack_ptr = 0;
size_t g_max_ptr = 0;

bool stack_init(void)
{
	if (g_stack) {
		return (true);
	}

	g_stack_ptr = 0;
	g_stack = array_create(sizeof(object_t), STACK_SIZE, NULL);

	if (!g_stack) {
		return (false);
	}

	array_settle(g_stack);
	return (true);
}

void stack_fini(void)
{
	array_kill(g_stack);
	g_stack_ptr = 0;
	g_max_ptr = 0;
	g_stack = NULL;
}

bool stack_push(object_t *object)
{
	g_stack_ptr++;
	g_max_ptr++;
	return (array_push(g_stack, object));
}

const object_t *stack_top(void)
{
	return (const object_t *)array_tail(g_stack);
}

void stack_pop(object_t *ptr)
{
	g_stack_ptr--;
	array_pop(g_stack, ptr);
	// if (ptr->dealloc) {
	// 	ptr->dealloc(ptr->data.ptr);
	// }
}

void stack_pop_discard(void)
{
	g_stack_ptr--;
	array_pop(g_stack, NULL);
}
