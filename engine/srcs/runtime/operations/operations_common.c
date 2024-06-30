#include "xre_operations.h"
#include "xre_assert.h"
#include "xre_object.h"
#include "xre_memory.h"
#include <stdbool.h>

void trigger_error_on(ast_stmt_t *self, error_type_e type)
{
	set_error_type(type);
	set_error_orig(self);
}

bool pop_binop_return(ast_stmt_t *self, object_t *left_buffer,
		      object_t *right_buffer)
{
	return (pop_object(right_buffer, RIGHT_BRANCH) &&
		pop_object(left_buffer, LEFT_BRANCH));
}

bool evaluate_binops(ast_stmt_t *self, object_t *left_buffer,
		     object_t *right_buffer)
{
	if (!BR_EVAL((LEFT_BRANCH)) || !BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	if (!pop_binop_return(self, left_buffer, right_buffer)) {
		return (false);
	}

	return (true);
}

bool stack_push_flagged(ast_stmt_t *self, object_t *object, int32_t flags)
{
	object->flags |= flags;
	if (!stack_push(object)) {
		trigger_error_on(self, XRE_STACK_OVERFLOW_ERROR);
		return (false);
	}
	return (true);
}
