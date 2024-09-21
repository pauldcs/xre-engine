#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_sequence(ast_stmt_t *self, object_t *object)
{
	static object_t lv;
	static object_t rv;

	if (!binop_evaluate_pop_r(self, &lv, &rv)) {
		return (false);
	}

	object_sequence_init(
		self->orig->_depth, &lv, &rv, object
	);
	
	return (true);
}

XRE_API(oper_sequence)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_sequence(self, &_result);
	return (ret ? __push_r(self, &_result) : false);
}
