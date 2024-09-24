#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_closure(struct statements *self)
{
	static object_t lv;

	if (!__br_eval(__left_branch) ||
	    !stack_pop_r(&lv, __left_branch)) {
		return (false);
	}

	object_attrs_repr(__object_get_attr(&lv));

	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR),
		false);
}

XRE_API(oper_closure)
{
	__trigger_bug_if(self == NULL);
	return (_oper_closure(self));
}
