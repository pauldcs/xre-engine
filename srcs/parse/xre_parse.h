#ifndef __XRE_PARSE_H__
# define __XRE_PARSE_H__

# include "array.h"
# include <sys/types.h>

typedef enum {
	/* <word> */ __IDENTIFIER__,
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
	/*     += */ __ADD_ASSIGN__,
	/*     -= */ __SUB_ASSIGN__,
	/*     *= */ __MUL_ASSIGN__,
	/*     /= */ __DIV_ASSIGN__,
	/*     %= */ __MOD_ASSIGN__,
	/*    ^^= */ __POW_ASSIGN__,
	/*     ?= */ __OR_ASSIGN__,
	/*     @= */ __AND_ASSIGN__,
	/*    <<= */ __LSHIFT_ASSIGN__,
	/*    >>= */ __RSHIFT_ASSIGN__,
	/*      , */ __SEQUENCE_POINT__,
	/*     <- */ __INJECT__,
	/*      : */ __ANNOTATE__,
	/*   loop */ __LOOP__,
	/*     do */ __DO__,
	/*   else */ __ELSE__,
	/*     :: */ __SCOPE_RESOLUTION__,
	/*      ! */ __NOT__,
	/*        */ __START__,
	/*        */ __END__,
}	t_xre_expr_kind;

typedef enum {
	EXPR_OP_TYPE_BINOP  = 1 << 1,
	EXPR_OP_TYPE_UNIOP  = 1 << 2,
	EXPR_TYPE_OPERAND   = 1 << 3,
	EXPR_TYPE_CONDITION = 1 << 4,
	EXPR_TYPE_LOOP      = 1 << 5,
	EXPR_TYPE_SEQUENCE  = 1 << 6,
	EXPR_TYPE_INJECT    = 1 << 7,
	EXPR_TYPE_OTHER     = 1 << 8,
}	t_xre_expr_type;

typedef struct s_xre_expr_token {
  t_xre_expr_type   _type;
	t_xre_expr_kind _kind;
	int64_t         _value;
	size_t          _line;
	size_t          _cols;
	const char *    _ptr;
	size_t          _len;
	const char *    _line_ptr;
	size_t          _line_len;
}	t_xre_token;

typedef struct s_ast t_xre_ast;

struct s_ast {
	t_xre_expr_type	type;
	t_xre_expr_kind	kind;
	const t_xre_token  token;

	union {
		int64_t value;
		const char *string;
		struct {
			t_xre_ast *left;	
			t_xre_ast *right;
		}	binop;
		t_xre_ast *uniop;
	};
};

t_xre_ast  *xre_ast_compose(const char *expr);
bool        xre_expr_lex (const char *expr, array_t *tokens);
bool        xre_expr_syntax (array_t *tokens);
t_xre_ast  *xre_expr_parse (array_t *tokens);

/*---      UTILS      ---*/
t_xre_expr_type  expr_type_by_kind(t_xre_expr_kind kind);
const char      *expr_kind_to_string(t_xre_expr_kind kind);
void             ast_show (t_xre_ast *ast);

#endif /* __XRE_PARSE_H__ */
