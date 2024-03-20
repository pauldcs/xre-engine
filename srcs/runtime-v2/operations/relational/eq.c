#include "xre_operations.h"
#include "xre_assert.h"

XRE_RELATIONAL_OPER_API(oper_eq)
{
	__return_val_if_fail__(lval, false);
	__return_val_if_fail__(rval, false);

	(void)rval;
	(void)rval;
	return (true);
}
