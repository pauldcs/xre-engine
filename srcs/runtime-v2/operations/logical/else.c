#include "xre_operations.h"
#include "xre_assert.h"

XRE_LOGICAL_OPER_API(oper_else)
{
	__return_val_if_fail__(condition, false);
	__return_val_if_fail__(consequence, false);

	(void)condition;
	(void)consequence;
	return (true);
}
