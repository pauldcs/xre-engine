#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_rshift(ast_stmt_t *self, object_t *object)
{
	static object_t lbuf;
	static object_t rbuf;

	if (!binop_evaluate_pop_r(self, &lbuf, &rbuf)) {
		return (false);
	}

	static int64_t a;
	static int64_t b;

	if (!unwrap_number_object(self, &lbuf, &a) ||
	    !unwrap_number_object(self, &rbuf, &b)) {
		return (false);
	}

	if (b > 64) {
		return (set_current_error(
				self, XRE_EXCEEDS_SHIFT_LIMIT_ERROR
			),
			false);
	}

	if (b < 0) {
		return (set_current_error(
				self, XRE_NEGATIVE_SHIFT_ERROR
			),
			false);
	}

	object_number_init(a >> b, object);
	return (true);
}

XRE_API(oper_rshift)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };
	
	bool ret = _oper_rshift(self, &_result);
	return (ret ? __push_rw(self, &_result) : false);
}
