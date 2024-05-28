#include "xre_args.h"
#include "xre_assert.h"
#include "xre_parse.h"

xre_ast_t *xre_ast_compose(const char *expr)
{
	__return_val_if_fail__(expr, NULL);

	array_t *tokens = NULL;
	xre_ast_t *ast = NULL;

	tokens = array_create(sizeof(xre_token_t), 16, NULL);
	if (!tokens)
		return (NULL);

	if (!xre_expr_lex(expr, tokens) || !xre_expr_syntax(tokens))
		goto prison;

	ast = xre_expr_parse(tokens);
	if (!ast)
		goto prison;

	/* success */
	goto beach;

prison:
	array_kill(tokens);
	return (NULL);

beach:
	if (__xre_args__.flags & FLAGS_DEBUG) {
		ast_show(ast);
	}
	array_kill(tokens);
	return (ast);
}
