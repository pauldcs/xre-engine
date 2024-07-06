#include "xre_errors.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stdint.h>

static void set_error_type(error_type_e type)
{
	_error.class = error_type_to_class(type);
	_error.type = type;
}

static void set_error_orig(ast_stmt_t *origin)
{
	_error.orig = origin->orig;
}

void set_current_error(ast_stmt_t *self, error_type_e type)
{
	set_error_type(type);
	set_error_orig(self);
}

void object_attrs_repr(int32_t attrs)
{
	const char *separator = "";

	if (attrs & ATTR_READABLE) {
		fprintf(stderr, "%sreadable", separator);
		separator = "-";
	}

	if (attrs & ATTR_MUTABLE) {
		fprintf(stderr, "%smutable", separator);
		separator = "-";
	}

	if (attrs & ATTR_REGISTER) {
		fprintf(stderr, "%sregister", separator);
		separator = "-";
	}

	if (attrs & ATTR_STRING) {
		fprintf(stderr, "%sstring", separator);
		separator = "-";
	}

	if (attrs & ATTR_SEQUENCE) {
		fprintf(stderr, "%ssequence", separator);
		separator = "-";
	}

	if (attrs & ATTR_SYMBOL) {
		fprintf(stderr, "%ssymbol", separator);
		separator = "-";
	}

	if (attrs & ATTR_REFERENCE) {
		fprintf(stderr, "%sreference", separator);
		separator = "-";
	}

	fprintf(stderr, "\n");
}

bool is_true_object(const object_t *object)
{
	__return_val_if_fail__(object, false);
	return (object->is_true(object->data.ptr));
}

void object_drop(void *ptr)
{
	(void)ptr;
	//((object_t *)ptr)->drop(((object_t *)ptr)->data.ptr);
}

bool stack_pop_readable(object_t *ptr, ast_stmt_t *stmts)
{
	stack_pop(ptr);

	if (!(ptr->attrs & ATTR_READABLE)) {
		return (set_current_error(stmts, XRE_UNREADABLE_ERROR), false);
	}

	return (true);
}

bool stack_pop_readable_binop(ast_stmt_t *self, object_t *left_buffer,
			      object_t *right_buffer)
{
	return (stack_pop_readable(right_buffer, __right_branch) &&
		stack_pop_readable(left_buffer, __left_branch));
}

bool self_evaluate_binop(ast_stmt_t *self, object_t *left_buffer,
			 object_t *right_buffer)
{
	if (!__br_eval(__left_branch) || !__br_eval(__right_branch) ||
	    !stack_pop_readable_binop(self, left_buffer, right_buffer)) {
		return (false);
	}

	return (true);
}

bool stack_push_enable_attrs(ast_stmt_t *self, object_t *object, int32_t attrs)
{
	object->attrs |= attrs;
	object->depth = self->orig->_depth;

	if (!stack_push(object)) {
		return (set_current_error(self, XRE_STACK_OVERFLOW_ERROR),
			false);
	}
	return (true);
}
