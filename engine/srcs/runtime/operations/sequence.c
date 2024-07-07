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

	if (!binop_evaluate_pop_r(self, &lv, &rv)) {
		return (false);
	}

	return (__push_r(self, object_create_sequence(&lv, &rv)));
}
