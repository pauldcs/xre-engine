#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_or(ast_stmt_t *self)
{
	if (!__br_eval(__left_branch)) {
		return (false);
	}

	if (is_true_object(stack_top())) {
		return (true);
	}

	stack_pop_discard();

	return (__br_eval(__right_branch));
}

XRE_API(oper_or)
{
	__trigger_bug_if(self == NULL);
	return (_oper_or(self));
}
