#include "xre_compiler.h"
#include "xre_errors.h"
#include "xre_parse.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#define PREV_TOKEN_KIND \
	((struct token *)vec_at(tokens, idx - 2))->_kind
#define PREV_TOKEN_TYPE \
	((struct token *)vec_at(tokens, idx - 2))->_type

err_notif_t syntax_error_g;

bool xre_expr_syntax(vec_t *tokens)
{
	__return_val_if_fail__(tokens, false);

	struct token *token  = NULL;
	size_t	      size   = tokens->_nmemb;
	size_t	      idx    = 0;
	int	      p_open = 0;
	int	      p_save = 0;
	int	      b_open = 0;

	(void)memset(&syntax_error_g, 0, sizeof(syntax_error_g));

	while (size--) {
		token	      = (struct token *)vec_at(tokens, idx++);
		token->_depth = p_open + b_open;

		if (!token)
			goto prison;

		switch (token->_kind) {
		case __START__:
			continue;

		case __LPAREN__:
			p_open++;
			p_save = b_open;
			goto as_value;

		case __LBRACK__:
			b_open++;
#if defined(__linux__)
			__attribute__((fallthrough));
#endif
		case __BUILTIN_CALL__:
			switch (token->_type) {
			case EXPR_OP_TYPE_BINOP:
				goto as_binop;

			case EXPR_OP_TYPE_UNIOP:
				goto as_uniop;

			case EXPR_TYPE_OTHER:
			case EXPR_OP_TYPE_SEQUENCE:
			case EXPR_TYPE_VALUE:
				break;
			}
		case __VAL__:
		case __STRING_LITERAL__:
		case __VARIABLE__:
		case __SEQUENCE__:
as_value:
			if (PREV_TOKEN_TYPE & (EXPR_OP_TYPE_BINOP |
					       EXPR_OP_TYPE_UNIOP) ||
			    PREV_TOKEN_KIND == __LBRACK__ ||
			    PREV_TOKEN_KIND == __LPAREN__ ||
			    PREV_TOKEN_KIND == __START__)
				continue;

			syntax_error_g.class = error_type_to_class(
				XRE_UNEXPECTED_OPERAND_ERROR
			);
			syntax_error_g.type =
				XRE_UNEXPECTED_OPERAND_ERROR;

			goto syntax_error;

		case __NOT__:
as_uniop:
			if (PREV_TOKEN_TYPE & (EXPR_OP_TYPE_BINOP |
					       EXPR_OP_TYPE_UNIOP) ||
			    PREV_TOKEN_KIND == __LBRACK__ ||
			    PREV_TOKEN_KIND == __LPAREN__ ||
			    PREV_TOKEN_KIND == __START__)
				continue;

			syntax_error_g.class = error_type_to_class(
				XRE_UNEXPECTED_OPERATOR_ERROR
			);
			syntax_error_g.type =
				XRE_UNEXPECTED_OPERATOR_ERROR;
			goto syntax_error;

		case __RPAREN__:
			if (p_open == 0 || b_open > p_save) {
				syntax_error_g.class =
					error_type_to_class(
						XRE_UNMATCHED_BRACKETS_ERROR
					);
				syntax_error_g.type =
					XRE_UNMATCHED_BRACKETS_ERROR;

				goto syntax_error;
			}
			p_open--;
			goto as_binop;

		case __RBRACK__:
			if (b_open == 0) {
				syntax_error_g.class =
					error_type_to_class(
						XRE_UNMATCHED_BRACKETS_ERROR
					);
				syntax_error_g.type =
					XRE_UNMATCHED_BRACKETS_ERROR;

				goto syntax_error;
			}
			b_open--;
#if defined(__linux__)
			__attribute__((fallthrough));
#endif

		case __ADD__:
		case __SUB__:
		case __MUL__:
		case __DIV__:
		case __POW__:
		case __MOD__:
		case __LT__:
		case __GT__:
		case __LE__:
		case __GE__:
		case __LSHIFT__:
		case __RSHIFT__:
		case __AND__:
		case __DO__:
		case __ELSE__:
		case __OR__:
		case __EQ__:
		case __NE__:
		case __BAND__:
		case __BOR__:
		case __BXOR__:
		case __SEQUENCE_POINT__:
		case __ATTRIBUTE__:
		case __SEPARATOR__:
		case __LOOP__:
		case __SCOPE_RESOLUTION__:
		case __END__:
		case __CLOSURE__:
		case __ASSIGN__:
as_binop:
			if (PREV_TOKEN_TYPE & EXPR_TYPE_VALUE ||
			    PREV_TOKEN_KIND == __RBRACK__ ||
			    PREV_TOKEN_KIND == __RPAREN__)
				continue;

			syntax_error_g.class = error_type_to_class(
				XRE_UNEXPECTED_OPERATOR_ERROR
			);
			syntax_error_g.type =
				XRE_UNEXPECTED_OPERATOR_ERROR;

			goto syntax_error;
		}

		__return_val_if_fail__(false, false);
	}

	if (b_open || p_open) {
		syntax_error_g.class = error_type_to_class(
			XRE_UNMATCHED_BRACKETS_ERROR
		);
		syntax_error_g.type = XRE_UNMATCHED_BRACKETS_ERROR;

		goto syntax_error;
	}

	return (true);

syntax_error:
	syntax_error_g.orig = token;
	xre_error(&syntax_error_g);

prison:
	return (false);
}
