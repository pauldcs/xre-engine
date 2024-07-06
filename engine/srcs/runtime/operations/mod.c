#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_mod)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!self_evaluate_binop(self, &lv, &rv)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_register_object(self, &lv, &a) ||
	    !unwrap_register_object(self, &rv, &b)) {
		return (false);
	}

	if (b == 0) {
		return (set_current_error(self, XRE_ZERO_DIVISION_ERROR),
			false);
	}

	return (stack_push_enable_attrs(self, object_create_register(a % b),
					ATTR_READABLE | ATTR_MUTABLE));
}
