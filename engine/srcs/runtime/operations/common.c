#include "xre_operations.h"
#include "xre_assert.h"
#include "xre_memory.h"
#include <stdbool.h>

void trigger_error_on(ast_stmt_t *self, error_type_e type)
{
	set_error_type(type);
	set_error_orig(self);
}

bool pop_object(object_t *ptr, ast_stmt_t *stmts)
{
	stack_pop(ptr);

	if (!(ptr->flags & FLAG_READABLE)) {
		return (trigger_error_on(stmts, XRE_UNREADABLE_ERROR), false);
	}

	return (true);
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

static inline bool extract_register_object(object_t *object, int64_t *data)
{
	if (!(object->flags & FLAG_REGISTER)) {
		return (false);
	}
	*data = __as_int64_t(object);
	return (true);
}

bool unwrap_register_object(ast_stmt_t *self, object_t *object, int64_t *data)
{
	if (!extract_register_object(object, data)) {
		return (trigger_error_on(self, XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}
	return (true);
}

inline static bool expand_symbol(ast_stmt_t *self, int64_t offset,
				 object_t **object)
{
	*object = array_access(g_symtab, offset);
	if (!object) {
		return (trigger_error_on(self, XRE_OUT_OF_BOUNDS_ACCESS_ERROR),
			false);
	}
	return (true);
}

bool expand_symbol_read(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!expand_symbol(self, offset, object)) {
		return (false);
	}
	if ((*object)->flags & FLAG_READABLE) {
		return (true);
	}

	return (trigger_error_on(self, XRE_UNREADABLE_ERROR), false);
}

bool expand_symbol_write(ast_stmt_t *self, int64_t offset, object_t **object)
{
	if (!expand_symbol(self, offset, object)) {
		return (false);
	}

	if (!((*object)->flags & FLAG_MUTABLE)) {
		return (trigger_error_on(self, XRE_WRITE_ON_READONLY_ERROR),
			false);
	}
	return (true);
}
