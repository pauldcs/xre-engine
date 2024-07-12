#ifndef __XRE_PARSE_H__
#define __XRE_PARSE_H__

#include "array.h"
#include <sys/types.h>

typedef enum {
	/* <word> */ __VARIABLE__,
	/* .<word> */ __BUILTIN_CALL__,
	/*     && */ __SCOPE_RESOLUTION__,
	/*   <n>  */ __VAL__,
	/*  "str" */ __STRING_LITERAL__,
	/*      ( */ __LPAREN__,
	/*      ) */ __RPAREN__,
	/*      + */ __ADD__,
	/*      - */ __SUB__,
	/*      * */ __MUL__,
	/*      / */ __DIV__,
	/*     ^^ */ __POW__,
	/*      % */ __MOD__,
	/*      < */ __LT__,
	/*      > */ __GT__,
	/*     <= */ __LE__,
	/*     >= */ __GE__,
	/*     << */ __LSHIFT__,
	/*     >> */ __RSHIFT__,
	/*     && */ __AND__,
	/*     || */ __OR__,
	/*     == */ __EQ__,
	/*     != */ __NE__,
	/*      & */ __BAND__,
	/*      | */ __BOR__,
	/*      ^ */ __BXOR__,
	/*      = */ __ASSIGN__,
	/*      , */ __SEQUENCE__,
	/*      ; */ __SEPARATOR__,
	/*   loop */ __LOOP__,
	/*     do */ __DO__,
	/*   else */ __ELSE__,
	/*      ! */ __NOT__,
	/*        */ __START__,
	/*        */ __END__,
} xre_expr_kind_t;

typedef enum {
	EXPR_OP_TYPE_BINOP = 1 << 1,
	EXPR_OP_TYPE_UNIOP = 1 << 2,
	EXPR_TYPE_VALUE = 1 << 3,
	EXPR_TYPE_OTHER = 1 << 4,
} xre_expr_type_t;

typedef struct s_xre_expr_token {
	xre_expr_type_t _type;
	xre_expr_kind_t _kind;
	int64_t _value;
	size_t _line;
	size_t _cols;
	const char *_ptr;
	size_t _len;
	const char *_line_ptr;
	size_t _line_len;
	size_t _depth;
} xre_token_t;

typedef struct s_ast xre_ast_t;

struct s_ast {
	xre_expr_type_t type;
	xre_expr_kind_t kind;
	const xre_token_t token;
	union {
		int64_t value;
		const char *string;
		xre_ast_t *uniop;
		struct {
			xre_ast_t *left;
			xre_ast_t *right;
		} _binop;
	};
};

xre_ast_t *xre_ast_compose(const char *expr);
bool xre_expr_lex(const char *expr, array_t *tokens);
bool xre_expr_syntax(array_t *tokens);
xre_ast_t *xre_expr_parse(array_t *tokens);

int get_precedence_by_kind(xre_expr_kind_t kind);

/*---      UTILS      ---*/
xre_expr_type_t expr_type_by_kind(xre_expr_kind_t kind);
const char *expr_kind_to_string(xre_expr_kind_t kind);
void ast_show(xre_ast_t *ast);
void ast_free(xre_ast_t *ast);

#endif /* __XRE_PARSE_H__ */
