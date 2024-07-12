#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_bw_and)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;

	if (!binop_evaluate_pop_r(self, &lv, &rv)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_number_object(self, &lv, &a) ||
	    !unwrap_number_object(self, &rv, &b)) {
		return (false);
	}

	return (__push_rw(self, object_number_create(a & b)));
}
