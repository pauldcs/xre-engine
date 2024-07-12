#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_eq)
{
	__return_val_if_fail__(self, false);

	static object_t lbuf;
	static object_t rbuf;

	if (!binop_evaluate_pop_r(self, &lbuf, &rbuf)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_number_object(self, &lbuf, &a) ||
	    !unwrap_number_object(self, &rbuf, &b)) {
		return (false);
	}

	return (__push_rw(self, object_number_create(a == b)));
}
