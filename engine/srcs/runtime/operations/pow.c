#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_pow(ast_stmt_t *self)
{
	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR),
		false);
}

XRE_API(oper_pow)
{
	__trigger_bug_if(self == NULL);
	return (_oper_pow(self));
}
