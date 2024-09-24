#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_lt(struct statements *self, object_t *object)
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

	object_number_init(a < b, object);
	return (true);
}

XRE_API(oper_lt)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_lt(self, &_result);
	return (ret ? __push_rw(self, &_result) : false);
}
