#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

XRE_API_OPERATOR_FUNC(oper_assign)
{
	__return_val_if_fail__(self, false);

	static object_t lv;
	static object_t rv;
	static object_t *object;

	if (!oper_symbol_addr(__left_branch) || !__br_eval(__right_branch) ||
	    !stack_pop_r_binop(self, &lv, &rv)) {
		return (false);
	}

	if (!unwrap_symbol_write(self, __object_get_data_as_symbol(&lv),
				 &object)) {
		return (false);
	}

	(void)memmove(object, &rv, sizeof(object_t));

	return (__push_r(self, &rv));
}
