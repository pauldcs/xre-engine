#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

static inline bool _oper_assign(struct statements *self)
{
	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR),
		false);
}

XRE_API(oper_assign)
{
	__trigger_bug_if(self == NULL);
	return (_oper_assign(self));
}
