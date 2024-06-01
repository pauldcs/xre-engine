#include "xre_runtime.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include <stdlib.h>

err_notif_t _error;
ast_stmt_t *__statements__ = NULL;

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

	if (!symtab_init()) {
		return ((void)fprintf(stderr, "Out of memory\n"), false);
	}

	ast_stmt_t *self = stmt_tree_create(ast);
	if (!self) {
		return ((void)fprintf(stderr, "Out of memory\n"), symtab_fini(),
			false);
	}

	if (!stack_init()) {
		return ((void)fprintf(stderr, "Out of memory\n"),
			stmt_tree_destroy(self), symtab_fini(), false);
	}

	__statements__ = self;

	if (!self->eval(self)) {
		xre_error(&_error);
		return (symtab_fini(), stmt_tree_destroy(self), stack_fini(),
			false);
	}

	//stack_debug();
	return (stmt_tree_destroy(self), symtab_fini(), stack_fini(), true);
}
