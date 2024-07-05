#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>

XRE_API_OPERATOR_FUNC(oper_do)
{
	__return_val_if_fail__(self, false);

	if (!oper_and(self)) {
		return (false);
	}

	__stack_top_disable_flags(FLAG_READABLE);
	return (true);
}
