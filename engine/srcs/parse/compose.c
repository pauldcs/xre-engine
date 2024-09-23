#include "xre_args.h"
#include "xre_compiler.h"
#include "xre_parse.h"

// void fuck_with_tokens(vec_t *tokens) {
// 	size_t size = vec_size(tokens);
// 	for (size_t i = 0; i < size; i++) {
// 		printf("'%s'\n", expr_kind_to_string(((xre_token_t *)vec_at(tokens, i))->_kind));
// 	}
// 	(void)tokens;
// }

xre_ast_t *xre_ast_compose(const char *expr)
{
	__return_val_if_fail__(expr, NULL);

	vec_t	  *tokens = NULL;
	xre_ast_t *ast	  = NULL;

	tokens = vec_create(sizeof(xre_token_t), 16, NULL);
	if (!tokens) {
		return (NULL);
	}

	if (!xre_expr_lex(expr, tokens) || !xre_expr_syntax(tokens)) {
		goto prison;
	}

	//fuck_with_tokens(tokens);

	ast = xre_expr_parse(tokens);
	if (!ast) {
		goto prison;
	}

	/* success */
	goto beach;

prison:
	vec_kill(tokens);
	return (NULL);

beach:
	if (__xre_args__.flags & FLAGS_DEBUG) {
		ast_show(ast);
	}

	vec_kill(tokens);
	return (ast);
}
