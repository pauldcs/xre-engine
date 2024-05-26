#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>
#include <string.h>

XRE_OPERATOR_API(oper_assign)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!oper_symbol_addr(LEFT_BRANCH)) {
		return (false);
	}

	if (!BR_EVAL((RIGHT_BRANCH))) {
		return (false);
	}

	stack_pop(&rv);
	stack_pop(&lv);

	object_t *o = array_access(g_symtab, VALUE_OF(int64_t, &lv));
	if (!o) {
		return (set_error_type(XRE_OUT_OF_BOUNDS_ACCESS_ERROR),
			set_error_orig(self), false);
	}
	(void)memmove(o, &rv, sizeof(object_t));

	if (!stack_push(&rv)) {
		return (set_error_type(XRE_STACK_OVERFLOW_ERROR),
			set_error_orig(self), false);
	}

	return (true);
}
