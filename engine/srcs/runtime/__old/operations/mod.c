#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool
_oper_mod(struct statements *self, object_t *object)
{
	static object_t lv;
	static object_t rv;

	if (!binop_evaluate_pop_r(self, &lv, &rv)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_number_object(self, &lv, &a) ||
	    !unwrap_number_object(self, &rv, &b)) {
		return (false);
	}

	if (b == 0) {
		return (set_current_error(
				self, XRE_ZERO_DIVISION_ERROR
			),
			false);
	}

	object_number_init(a % b, object);
	return (true);
}

XRE_API(oper_mod)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_mod(self, &_result);
	return (ret ? __push_rw(self, &_result) : false);
}
