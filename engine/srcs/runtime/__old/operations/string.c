#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>
#include <string.h>

static inline bool
_oper_string(struct statements *self, object_t *object)
{
	return (object_string_init(
		(unsigned char *)self->string, object
	));
}

XRE_API(oper_string)
{
	__trigger_bug_if(self == NULL);
	static object_t _result = { 0 };

	bool ret = _oper_string(self, &_result);
	return (ret ? __push_r(self, &_result) : false);
}
