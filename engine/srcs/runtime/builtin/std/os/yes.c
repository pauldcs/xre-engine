#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_yes)
{
	__return_val_if_fail__(self, false);
	return (__push_r(self,
			 object_string_create((unsigned char *)"yes", 3)));
}
