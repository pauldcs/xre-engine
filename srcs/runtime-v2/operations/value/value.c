#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_log.h"

bool value_initiator(stmt_tree_t *tree)
{
	__return_val_if_fail__(tree, false);
	XRE_LOGGER(debug, "value_initiator");

	set_error(tree, XRE_NOT_IMPLEMENTED_ERROR);
	return (false);
}