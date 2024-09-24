#include "xre_parse.h"
#include "xre_compiler.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool runtime_alloc(struct runtime **rt)
{
	*rt = malloc(sizeof(struct runtime));
	if (!*rt) {
		return (false);
	}
	(void)memset(*rt, 0x00, sizeof(struct runtime));
	return (true);
}

void runtime_dealloc(struct runtime **rt)
{
	(void)free((void *)*rt);
	*rt = NULL;
}

static bool runtime_init(xre_ast_t *ast, struct runtime **rt)
{
	(void)ast;
	if (!runtime_alloc(rt)) {
		return (false);
	}

	if (!runtime_tree_init(ast, *rt)) {
		goto prison;
	}

	// return (true);
	// if (!alloc_zeroed_stmt_tree(
	// 	    get_tree_size(ast) * sizeof(struct statements),
	// 	    buffer
	//     )) {
	// 	return (false);
	// }

	// (void)stmt_tree_init(*buffer, ast, true);
	return (true);
prison:
	runtime_dealloc(rt);
	return (false);
}

bool runtime(xre_ast_t *ast)
{
	__trigger_bug_if(ast == false);

	struct runtime *rt = NULL;

	if (!runtime_init(ast, &rt)) {
		return (false);
	}

	runtime_tree_put(rt->start);

	return (true);
	// 	if (!stack_init()) {
	// 		stmt_tree_destroy(self);
	// 		goto out_of_memory;
	// 	}

	// 	// if (!heap_init()) {
	// 	// 	stmt_tree_destroy(self);
	// 	// 	goto out_of_memory;
	// 	// }

	// 	__statements__ = self;

	// 	scoper(self);
	// 	if (!self->eval(self)) {
	// 		return (xre_error(&_error),
	// 			stmt_tree_destroy(self),
	// 			false);
	// 	}

	// 	stack_pop(&result);

	// 	if (__xre_args__.flags & SHOW_EXPR_RESULT) {
	// 		result.repr(__object_get_data_as_any(&result));
	// 		(void)fprintf(stderr, "\n");
	// 	}

	// 	return (object_drop(&result), stmt_tree_destroy(self), true);

	// out_of_memory:
	// 	(void)fprintf(stderr, "Out of memory\n");
	// 	return (false);
}
