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
	return (pop_object(right_buffer, __right_branch) &&
		pop_object(left_buffer, __left_branch));
}

bool evaluate_binops(ast_stmt_t *self, object_t *left_buffer,
		     object_t *right_buffer)
{
	if (!__br_eval(__left_branch) || !__br_eval(__right_branch) ||
	    !pop_binop_return(self, left_buffer, right_buffer)) {
		return (false);
	}

	return (true);
}

bool stack_push_flagged(ast_stmt_t *self, object_t *object, int32_t flags)
{
	object->flags |= flags;
	object->depth = self->orig->_depth;

	if (!stack_push(object)) {
		return (trigger_error_on(self, XRE_STACK_OVERFLOW_ERROR),
			false);
	}
	return (true);
}
