#include "xre_operations.h"
#include "xre_assert.h"

XRE_ASSIGNMENT_OPER_API(oper_mul_assign)
{
	__return_val_if_fail__(dst, false);
	__return_val_if_fail__(src, false);

	(void)dst;
	(void)src;
	return (true);
}
