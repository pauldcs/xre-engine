#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

static inline bool _oper_assign(ast_stmt_t *self, object_t *object)
{
	static object_t lv;
	static object_t rv;

	if (!oper_symbol_addr(__left_branch) ||
	    !__br_eval(__right_branch) ||
	    !stack_pop_r_binop(self, &lv, &rv)) {
		return (false);
	}

	if (!unwrap_symbol_write(
		    self, __object_get_data_as_symbol(&lv), &object
	    )) {
		return (false);
	}

	(void)memmove(object, &rv, sizeof(object_t));
	return (__push_r_as_ref(self, &rv));
}

XRE_API(oper_assign)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	return (_oper_assign(self, &_result));
}
