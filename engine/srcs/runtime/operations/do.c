#include "xre_assert.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

/* This does not work as "if" is supposed
 * to return the condition. 
 */
XRE_API_OPERATOR_FUNC(oper_do)
{
	return (oper_and(self));
}
