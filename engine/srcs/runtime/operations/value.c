#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_API(oper_value)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	object_number_init(self->value, &_result);
	return (__push_rw(self, &_result));
}
