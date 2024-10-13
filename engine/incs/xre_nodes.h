#ifndef __XRE_NODES_H__
#define __XRE_NODES_H__

#include "vec.h"
#include <stdint.h>

enum expr_kind {
	/* <word> */ __VARIABLE__,
	/* .<word> */ __BUILTIN_CALL__,
	/*     :: */ __SCOPE_RESOLUTION__,
	/*   <n>  */ __VAL__,
	/*  "str" */ __STRING_LITERAL__,
	/*      { */ __LBRACK__,
	/*      } */ __RBRACK__,
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
	/*     => */ __CLOSURE__,
	/*     != */ __NE__,
	/*      & */ __BAND__,
	/*      | */ __BOR__,
	/*      ^ */ __BXOR__,
	/*      = */ __ASSIGN__,
	/*      , */ __SEQUENCE_POINT__,
	/*        */ __SEQUENCE__,
	/*      ; */ __SEPARATOR__,
	/*      : */ __ATTRIBUTE__,
	/*   loop */ __LOOP__,
	/*     do */ __DO__,
	/*   else */ __ELSE__,
	/*      ! */ __NOT__,
	/*        */ __START__,
	/*        */ __END__,
};

enum expr_type {
	EXPR_OP_TYPE_BINOP    = 1 << 1,
	EXPR_OP_TYPE_UNIOP    = 1 << 2,
	EXPR_OP_TYPE_SEQUENCE = 1 << 3,
	EXPR_TYPE_VALUE	      = 1 << 4,
	EXPR_TYPE_OTHER	      = 1 << 5,
};

enum expr_type expr_type_by_kind(enum expr_kind kind);
const char    *expr_kind_to_string(enum expr_kind kind);
const char    *expr_type_to_string(enum expr_type type);

struct token {
	enum expr_type _type;
	enum expr_kind _kind;
	int64_t	       _value;
	size_t	       _line;
	size_t	       _cols;
	const char    *_ptr;
	size_t	       _len;
	const char    *_line_ptr;
	size_t	       _line_len;
	size_t	       _depth;
};

struct ast {
	enum expr_type	   type;
	enum expr_kind	   kind;
	const struct token token;
	union {
		int64_t	       value;
		const char    *string;
		struct vector *seq;
		struct ast    *uniop;
		struct {
			struct ast *left;
			struct ast *right;
		} _binop;
	};
};

#endif /* __XRE_NODES_H__ */
