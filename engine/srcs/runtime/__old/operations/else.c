#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API(oper_else)
{
	__trigger_bug_if(self == NULL);
	return (oper_or(self));
}
