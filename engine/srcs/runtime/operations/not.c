#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_not(struct statement *self, object_t *object)
{
	static object_t v;

	if (!__br_eval(__left_branch) ||
	    !stack_pop_r(&v, __left_branch)) {
		return (false);
	}

	object_number_init(!is_true_object(&v), object);
	return (true);
}

XRE_API(oper_not)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_not(self, &_result);
	return (ret ? __push_rw(self, &_result) : false);
}
