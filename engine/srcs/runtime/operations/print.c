#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_print)
{
	__return_val_if_fail__(self, false);

	object_t v;
	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	stack_pop(&v);

	v.repr(v.data.ptr);

	if (!stack_push(object_create_register((int64_t)v.data.ptr))) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}
	return (true);
}
