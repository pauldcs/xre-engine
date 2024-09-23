#include "xre_runtime.h"
#include "xre_compiler.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include "xre_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static bool stmt_tree_create(xre_ast_t *ast, struct statement **buffer)
{
	size_t get_tree_size(ast) * sizeof(struct statement)
	size_t alloc_size = sizeof(struct statement) * size;
	*buffer		  = malloc(alloc_size);
	if (!*buffer) {
		return (false);
	}

	(void)memset(*buffer, 0x00, alloc_size);
	return (true);
	if (!alloc_zeroed_stmt_tree(
		    get_tree_size(ast) * sizeof(struct statement), buffer
	    )) {
		return (false);
	}

	(void)stmt_tree_init(*buffer, ast, true);
	return (true);
}

bool xre_runtime(xre_ast_t *ast)
{
	__trigger_bug_if(ast == false);

	struct statement *self = NULL;

	if (!stmt_tree_create(ast, &self)) {
		goto out_of_memory;
	}

	if (!stack_init()) {
		stmt_tree_destroy(self);
		goto out_of_memory;
	}

	// if (!heap_init()) {
	// 	stmt_tree_destroy(self);
	// 	goto out_of_memory;
	// }

	__statements__ = self;

	scoper(self);
	if (!self->eval(self)) {
		return (xre_error(&_error),
			stmt_tree_destroy(self),
			false);
	}

	stack_pop(&result);

	if (__xre_args__.flags & SHOW_EXPR_RESULT) {
		result.repr(__object_get_data_as_any(&result));
		(void)fprintf(stderr, "\n");
	}

	return (object_drop(&result), stmt_tree_destroy(self), true);

out_of_memory:
	(void)fprintf(stderr, "Out of memory\n");
	return (false);
}
