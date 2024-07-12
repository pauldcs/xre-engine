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

	if (attrs & ATTR_CONSTANT) {
		fprintf(stderr, "%sconstant", separator);
		separator = "-";
	}

	if (attrs & ATTR_READABLE) {
		fprintf(stderr, "%sreadable", separator);
		separator = "-";
	}

	if (attrs & ATTR_MUTABLE) {
		fprintf(stderr, "%smutable", separator);
		separator = "-";
	}

	if (attrs & ATTR_NUMBER) {
		fprintf(stderr, "%snumber", separator);
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
	return (object->is_true(__object_get_data_as_any(object)));
}

void object_drop(void *ptr)
{
	__return_if_fail__(ptr);

	object_t *object = (object_t *)ptr;

	if (!__object_has_attr(object, ATTR_REFERENCE)) {
		object->drop(__object_get_data_as_any(object));
	}
}

bool stack_pop_r(object_t *ptr, ast_stmt_t *stmts)
{
	stack_pop(ptr);
	if (!__object_has_attr(ptr, ATTR_READABLE)) {
		return (set_current_error(stmts, XRE_UNREADABLE_ERROR), false);
	}

	// (void)object_delete(ptr);

	return (true);
}

bool stack_pop_r_binop(ast_stmt_t *self, object_t *left_buffer,
		       object_t *right_buffer)
{
	return (stack_pop_r(right_buffer, __right_branch) &&
		stack_pop_r(left_buffer, __left_branch));
}

bool binop_evaluate_pop_r(ast_stmt_t *self, object_t *left_buffer,
			  object_t *right_buffer)
{
	if (!__br_eval(__left_branch) || !__br_eval(__right_branch) ||
	    !stack_pop_r_binop(self, left_buffer, right_buffer)) {
		return (false);
	}

	return (true);
}

bool stack_push_enable_attrs(ast_stmt_t *self, object_t *object, int32_t attrs)
{
	__object_set_attr(object, attrs);
	__object_set_depth(object, self->orig->_depth);

	// if (!object_new(object)) {
	// 	return (set_current_error(self, XRE_OUT_OF_MEMORY_ERROR),
	// 		false);
	// }

	if (!stack_push(object)) {
		// (void)object_delete(object);
		return (set_current_error(self, XRE_STACK_OVERFLOW_ERROR),
			false);
	}

	return (true);
}
