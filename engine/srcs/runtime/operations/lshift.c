#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_lshift)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;
	static int32_t data;

	if (!evaluate_binops(self, &lv, &rv)) {
		return (false);
	}

	if (VALUE_OF(int64_t, &rv) > 64) {
		return (trigger_error_on(self, XRE_EXCEEDS_SHIFT_LIMIT_ERROR),
			false);
	}

	if (VALUE_OF(int64_t, &rv) < 0) {
		return (trigger_error_on(self, XRE_NEGATIVE_SHIFT_ERROR),
			false);
	}

	data = VALUE_OF(int64_t, &lv) << VALUE_OF(int64_t, &rv);
	return (stack_push_flagged(self, object_create_register(data),
				   FLAG_READABLE | FLAG_MUTABLE));
}
