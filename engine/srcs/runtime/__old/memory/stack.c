#include "vec.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

vec_t *g_stack	   = NULL;
size_t g_stack_ptr = 0;
size_t g_max_ptr   = 0;

bool stack_init(void)
{
	if (g_stack) {
		return (true);
	}

	g_stack_ptr = 0;
	g_stack	    = vec_create(sizeof(object_t), STACK_SIZE, NULL);

	if (!g_stack) {
		return (false);
	}

	//vec_settle(g_stack);

	return (true);
}

void stack_fini(void)
{
	vec_kill(g_stack);

	g_stack_ptr = 0;
	g_max_ptr   = 0;
	g_stack	    = NULL;
}

bool stack_push(object_t *object)
{
	if (!vec_push(g_stack, object)) {
		return (false);
	}

	g_stack_ptr++;
	g_max_ptr++;

	return (true);
}

const object_t *stack_top(void)
{
	return (const object_t *)vec_tail(g_stack);
}

void stack_pop(object_t *ptr)
{
	vec_pop(g_stack, ptr);

	g_stack_ptr--;
}

void stack_pop_discard(void)
{
	object_t object;

	stack_pop(&object);
	if (!__object_has_attr(&object, ATTR_REFERENCE))
		object_drop(&object);

	g_stack_ptr--;
}
