#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_loop(struct statement *self)
{
	__return_val_if_fail__(self, false);

loop:
	if (!__br_eval(__left_branch)) {
		return (false);
	}

	if (!is_true_object(stack_top())) {
		return (true);
	}

	stack_pop_discard();

	if (!__br_eval(__right_branch)) {
		return (false);
	}

	stack_pop_discard();

	goto loop;
}

XRE_API(oper_loop)
{
	__trigger_bug_if(self == NULL);
	return (_oper_loop(self));
}
