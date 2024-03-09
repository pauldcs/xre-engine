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
	/*      ; */ __SEPARATOR__,
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
	EXPR_TYPE_VALUE     = 1 << 3,
	EXPR_TYPE_OTHER     = 1 << 4,
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
}	xre_token_t;

typedef struct s_ast xre_ast_t;

typedef enum {
	STATE_ARRAY,
	STATE_NUMBER,
	STATE_STRING,
}	exp_event_e;

typedef struct {
  exp_event_e  type;
  union {
    int64_t    value;
    array_t    *array;
    const char *string;
};
} state_t;

struct s_ast {
	t_xre_expr_type	   type;
	t_xre_expr_kind	   kind;
	const xre_token_t  token;
	state_t            event;
	union {
		int64_t    value;
		const char *string;
		xre_ast_t  *uniop;
		struct {
			xre_ast_t *left;	
			xre_ast_t *right;
		}	_binop;
	};
};

xre_ast_t  *xre_ast_compose(const char *expr);
bool        xre_expr_lex (const char *expr, array_t *tokens);
bool        xre_expr_syntax (array_t *tokens);
xre_ast_t  *xre_expr_parse (array_t *tokens);

/*---      UTILS      ---*/
t_xre_expr_type  expr_type_by_kind(t_xre_expr_kind kind);
const char      *expr_kind_to_string(t_xre_expr_kind kind);
void             ast_show (xre_ast_t *ast);
void             ast_free(xre_ast_t *ast);

typedef struct {
  const char  *key;
  state_t     state;
} stack_item_t;

extern array_t *runtime_stack;

// FRAME
bool    runtime_stack_init(void);
void    runtime_stack_deinit(void);

state_t *runtime_stack_get(const char *key);

bool runtime_stack_add(const char *key, state_t state);
bool runtime_stack_set(const char *key, state_t state);

bool is_truthy_state(xre_ast_t *node);

// COMMON
void state_print(xre_ast_t *node);

#endif /* __XRE_PARSE_H__ */
