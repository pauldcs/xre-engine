#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_OPERATOR_API(oper_sequence)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!BR_EVAL((LEFT_BRANCH)) || !BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	stack_pop(&rv);
	stack_pop(&lv);

	if (!stack_push(object_create_sequence(&lv, &rv))) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}

	return (true);
}
