#include "xre_parse.h"
#include "xre_args.h"
#include "xre_assert.h"

t_xre_ast
*xre_ast_compose(const char *expr) {
  __return_val_if_fail__(expr, NULL);

  array_t *tokens = NULL;
  t_xre_ast *ast = NULL;

  tokens = array_create(
    sizeof(t_xre_token),
    16,
    NULL
  );
  if (!tokens)
    return (NULL);

  if (!xre_expr_lex(expr, tokens)
    || !xre_expr_syntax(tokens))
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
