#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_sequence)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!self_evaluate_binop(self, &lv, &rv)) {
		return (false);
	}

	if (!stack_push_enable_attrs(self, object_create_sequence(&lv, &rv),
				     ATTR_READABLE)) {
		return (false);
	}

	return (true);
}
