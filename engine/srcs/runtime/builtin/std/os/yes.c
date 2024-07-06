#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_yes)
{
	__return_val_if_fail__(self, false);

	return (!stack_push_enable_attrs(self, object_create_register(42),
					 ATTR_READABLE));
}
