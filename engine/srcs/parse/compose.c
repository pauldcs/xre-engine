#include "xre_args.h"
#include "xre_compiler.h"
#include "xre_parse.h"
#include "xre_nodes.h"

// void fuck_with_tokens(struct vector *tokens) {
// 	size_t size = vec_size(tokens);
// 	for (size_t i = 0; i < size; i++) {
// 		printf("'%s'\n", expr_kind_to_string(((struct token *)vec_at(tokens, i))->_kind));
// 	}
// 	(void)tokens;
// }

struct ast *xre_ast_compose(const char *expr)
{
	__return_val_if_fail__(expr, NULL);

	struct vector *tokens = NULL;
	struct ast    *ast    = NULL;

	tokens = vec_create(sizeof(struct token), 16, NULL);
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

	vec_kill(tokens);
	return (ast);
}
