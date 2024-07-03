#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "xre_log.h"
#include <stdbool.h>
#include <string.h>

XRE_API_OPERATOR_FUNC(oper_assign)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;
	static object_t *object;

	if (!oper_symbol_addr(LEFT_BRANCH) || !BR_EVAL((RIGHT_BRANCH)) ||
	    !pop_binop_return(self, &lv, &rv)) {
		return (false);
	}

	object = array_access(g_symtab, VALUE_OF(int64_t, &lv));
	if (!object) {
		return (trigger_error_on(self, XRE_OUT_OF_BOUNDS_ACCESS_ERROR),
			false);
	}

	if (!(object->flags & FLAG_MUTABLE)) {
		return (trigger_error_on(self, XRE_WRITE_ON_READONLY_ERROR),
			false);
	}

	(void)memmove(object, &rv, sizeof(object_t));

	return (stack_push_flagged(self, &rv, 0));
}
