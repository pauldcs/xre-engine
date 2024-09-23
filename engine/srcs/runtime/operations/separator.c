#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_separator(struct statement *self)
{
	if (!__br_eval(__left_branch)) {
		return (false);
	}

	stack_pop_discard();

	return (__br_eval(__right_branch));
}

XRE_API(oper_separator)
{
	__trigger_bug_if(self == NULL);
	return (_oper_separator(self));
}
