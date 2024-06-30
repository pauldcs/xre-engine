#include "xre_operations.h"
#include "xre_assert.h"
#include "xre_object.h"
#include "xre_memory.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_add)
{
	__return_val_if_fail__(self, false);

	static object_t lbuf;
	static object_t rbuf;
	static int32_t data;

	if (!evaluate_binops(self, &lbuf, &rbuf)) {
		return (false);
	}

	data = VALUE_OF(int64_t, &lbuf) + VALUE_OF(int64_t, &rbuf);
	return (stack_push_flagged(self, object_create_register(data),
				   FLAG_READABLE | FLAG_MUTABLE));
}
