#include "xre_runtime.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include <stdlib.h>

err_notif_t _error;
ast_stmt_t *__global_current_stmts_ptr__ = NULL;

void set_error_type(error_type_e type)
{
	_error.class = error_type_to_class(type);
	_error.type = type;
}

void set_error_orig(ast_stmt_t *origin)
{
	_error.orig = origin->orig;
}

bool xre_runtime(xre_ast_t *ast)
{
	__return_val_if_fail__(ast, false);

	ast_stmt_t *self = NULL;

	if (!symtab_init() || !stmt_tree_create(ast, &self)) {
		goto out_of_memory;
	}

	if (!stack_init()) {
		stmt_tree_destroy(self);
		goto out_of_memory;
	}

	__global_current_stmts_ptr__ = self;

	if (!self->eval(self)) {
		return (xre_error(&_error), stmt_tree_destroy(self), false);
	}

	//stack_debug();
	return (stmt_tree_destroy(self), true);

out_of_memory:
	(void)fprintf(stderr, "Out of memory\n");
	return (false);
}
