#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_bw_and)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!evaluate_binops(self, &lv, &rv)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_register_object(self, &lv, &a) ||
	    !unwrap_register_object(self, &rv, &b)) {
		return (false);
	}

	return (stack_push_flagged(self, object_create_register(a & b),
				   FLAG_READABLE | FLAG_MUTABLE));
}
