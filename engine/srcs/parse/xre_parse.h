#ifndef __XRE_PARSE_H__
#define __XRE_PARSE_H__

#include "vec.h"
#include "xre_nodes.h"
#include <sys/types.h>

struct ast *xre_ast_compose(const char *expr);
bool	    xre_expr_lex(const char *expr, vec_t *tokens);
bool	    xre_expr_syntax(vec_t *tokens);
struct ast *xre_expr_parse(vec_t *tokens);

int get_precedence_by_kind(enum expr_kind kind);

/*---      UTILS      ---*/
enum expr_type expr_type_by_kind(enum expr_kind kind);
const char    *expr_kind_to_string(enum expr_kind kind);
const char    *expr_type_to_string(enum expr_type type);
void	       ast_show(struct ast *ast);
void	       ast_free(struct ast *ast);

#endif /* __XRE_PARSE_H__ */
