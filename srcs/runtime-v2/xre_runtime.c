#include "xre_runtime.h"
#include "stdlib.h"

err_notif_t _error;

void set_error(stmt_tree_t *origin, error_type_e type)
{
	_error.class = error_type_to_class(type);
	_error.type = type;
	_error.orig = origin->orig;
}

bool xre_runtime(xre_ast_t *ast)
{
	stmt_tree_t *stmt = stmt_tree_init(ast);
	if (!stmt) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

	if (!stmt->op.init(stmt)) {
		xre_error(&_error);
		return (false);
	}

	// if (__xre_args__.flags & SHOW_EXPR_RESULT) {
	// 	state_print(frame);
	// }

	stmt_tree_fini(stmt);

	return (true);
}
