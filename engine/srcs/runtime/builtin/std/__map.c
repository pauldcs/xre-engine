#include "xre_compiler.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_map)
{
	return (set_current_error(self, XRE_NOT_IMPLEMENTED_ERROR),
		false);
}
