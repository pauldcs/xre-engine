#include "vec.h"
#include "xre_compiler.h"
#include "xre_errors.h"
#include "xre_runtime.h"
#include "xre_nodes.h"
#include "xre_utils.h"
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

struct error lexer_error_g;

struct token _token;
size_t	     _line;
size_t	     _cols;
char	    *_token_ptr;
char	    *_line_ptr;
size_t	     _line_len;

static size_t get_cur_line_len(void)
{
	size_t size = 0;
	char  *ptr  = _token_ptr;
	while (*ptr && *ptr != '\n') {
		ptr++;
		size++;
	}
	return (size);
}

static void init_token(void)
{
	_token._line	 = _line;
	_token._cols	 = _cols;
	_token._ptr	 = _token_ptr;
	_token._len	 = 0;
	_token._line_ptr = _line_ptr;
	_token._line_len = _line_len;
}

static bool accept_token(struct vector *tokens, size_t len)
{
	__return_val_if_fail__(tokens, false);

	_token._len = len;
	if (_token._kind == __BUILTIN_CALL__) {
		struct builtin *builtin =
			builtin_find(_token._ptr, _token._len);
		_token._type = builtin->type;

	} else {
		_token._type = expr_type_by_kind(_token._kind);
	}

	if (!vec_push(tokens, &_token))
		return (false);

	return (true);
}

static void pointer_forward(size_t count, char **ptr)
{
	while (count--) {
		if (**ptr == '\n') {
			_line_ptr  = (*ptr) + 1;
			_token_ptr = (*ptr) + 1;
			_line_len  = get_cur_line_len();
			_cols	   = 0;
			_line++;
		} else
			_cols++;

		_token_ptr = ++(*ptr);
	}
}

bool xre_expr_lex(const char *expr, struct vector *tokens)
{
	__return_val_if_fail__(expr, false);
	__return_val_if_fail__(tokens, false);

	char *ptr = (char *)expr;
	char *tmp = NULL;

	size_t tf;

	_line	     = 1;
	_cols	     = 0;
	_token_ptr   = ptr;
	_line_ptr    = ptr;
	_line_len    = get_cur_line_len();
	_token._kind = __START__;

	(void)memset(&lexer_error_g, 0, sizeof(lexer_error_g));

	init_token();
	if (!accept_token(tokens, 0)) {
		lexer_error_g.class =
			error_type_to_class(XRE_OUT_OF_MEMORY_ERROR);
		lexer_error_g.type = XRE_OUT_OF_MEMORY_ERROR;

		// assert(false);
		goto lexer_error;
	}

	while (*ptr) {
		tf = 0;

		while (*ptr && isspace(*ptr))
			pointer_forward(1, &ptr);

		if (!*ptr)
			break;

		if (isdigit(*ptr) || *ptr == '-') {
			int   result = 0;
			char *tmp    = ptr;

			init_token();
			tmp = str_to_int32(ptr, &result);
			if (!tmp) {
				lexer_error_g.class =
					error_type_to_class(
						XRE_OVERFLOW_ERROR
					);
				lexer_error_g.type =
					XRE_OVERFLOW_ERROR;

				// assert(false);
				goto lexer_error;
			}

			if (tmp == ptr) {
				/*
         *    The character sequence started by a digit or '-'
         *    but wasn't actually a number.
         */
				goto not_a_constant_value;
			}

			_token._kind  = __VAL__;
			_token._value = result;

			tf = tmp - ptr;

		} else {
not_a_constant_value:

			init_token();

			switch (*ptr) {
			case '*':
				_token._kind = __MUL__;
				tf	     = 1;
				break;

			case '/':
				_token._kind = __DIV__;
				tf	     = 1;
				break;

			case '%':
				_token._kind = __MOD__;
				tf	     = 1;
				break;

			case '{':
				_token._kind = __LBRACK__;
				tf	     = 1;

				break;
			case '}':
				_token._kind = __RBRACK__;
				tf	     = 1;

				break;
			case '(':
				_token._kind = __LPAREN__;
				tf	     = 1;

				break;
			case ')':
				_token._kind = __RPAREN__;
				tf	     = 1;

				break;
			case ',':
				_token._kind = __SEQUENCE_POINT__;
				tf	     = 1;

				break;
			case ';':
				_token._kind = __SEPARATOR__;
				tf	     = 1;

				break;

			case '=':
				if (*(ptr + 1) == '=') {
					_token._kind = __EQ__;
					tf	     = 2;
				} else if (*(ptr + 1) == '>') {
					_token._kind = __CLOSURE__;
					tf	     = 2;
				} else {
					_token._kind = __ASSIGN__;
					tf	     = 1;
				}

				break;
			case '!':
				if (*(ptr + 1) == '=') {
					_token._kind = __NE__;
					tf	     = 2;
				} else {
					_token._kind = __NOT__;
					tf	     = 1;
				}

				break;
			case '+':
				_token._kind = __ADD__;
				tf	     = 1;
				break;

			case '-':
				_token._kind = __SUB__;
				tf	     = 1;
				break;

			case '&':
				if (*(ptr + 1) == '&') {
					_token._kind = __AND__;
					tf	     = 2;
				} else {
					_token._kind = __BAND__;
					tf	     = 1;
				}

				break;

			case ':':
				if (*(ptr + 1) == ':') {
					_token._kind =
						__SCOPE_RESOLUTION__;
					tf = 2;
					break;
				} else {
					_token._kind = __ATTRIBUTE__;
					tf	     = 1;

					break;
				}

			case '|':
				if (*(ptr + 1) == '|') {
					_token._kind = __OR__;
					tf	     = 2;
				} else {
					_token._kind = __BOR__;
					tf	     = 1;
				}

				break;
			case '^':
				if (*(ptr + 1) == '^') {
					_token._kind = __POW__;
					tf	     = 2;
				} else {
					_token._kind = __BXOR__;
					tf	     = 1;
				}

				break;
			case '<':
				if (*(ptr + 1) == '<') {
					_token._kind = __LSHIFT__;
					tf	     = 2;
				} else if (*(ptr + 1) == '=') {
					_token._kind = __LE__;
					tf	     = 2;
				} else {
					_token._kind = __LT__;
					tf	     = 1;
				}

				break;
			case '>':
				if (*(ptr + 1) == '>') {
					_token._kind = __RSHIFT__;
					tf	     = 2;
				} else if (*(ptr + 1) == '=') {
					_token._kind = __GE__;
					tf	     = 2;
				} else {
					_token._kind = __GT__;
					tf	     = 1;
				}

				break;
			case '"':
				_token._kind = __STRING_LITERAL__;
				tmp	     = ptr;
				tf++;
				while (*tmp && *(tmp + 1) != '"') {
					tf++;
					tmp++;
				}

				if (!*tmp) {
					lexer_error_g.class =
						error_type_to_class(
							XRE_UNTERMINATED_STRING_ERROR
						);
					lexer_error_g.type =
						XRE_UNTERMINATED_STRING_ERROR;

					// assert(false);
					goto lexer_error;
				} else {
					tf++;
				}

				break;
			default:
				/* handle comments
TODO
*/
				// if (!strncmp(ptr, "mut", 3)) {
				// 	_token._kind = __MUT__;
				// 	tf = 3;
				// }

				if (!strncmp(ptr, "do", 2)) {
					_token._kind = __DO__;
					tf	     = 2;
				}

				else if (!strncmp(ptr, "else", 4)) {
					_token._kind = __ELSE__;
					tf	     = 4;
				}

				else if (!strncmp(ptr, "loop", 4)) {
					_token._kind = __LOOP__;
					tf	     = 4;
				}

				else {
					_token._kind = __VARIABLE__;
					tmp	     = ptr;
					tf++;
					while (*tmp &&
					       (isalnum(*(tmp + 1)) ||
						*(tmp + 1) == '_')) {
						tf++;
						tmp++;
					}

					if (builtin_find(ptr, tf)) {
						_token._kind =
							__BUILTIN_CALL__;
					}
				}
			}
		}

		if (!accept_token(tokens, tf)) {
			lexer_error_g.class = error_type_to_class(
				XRE_OUT_OF_MEMORY_ERROR
			);
			lexer_error_g.type = XRE_OUT_OF_MEMORY_ERROR;

			// assert(false);
			goto lexer_error;
		}

		__return_val_if_fail__(tf, false);
		pointer_forward(tf, &ptr);
	}

	_token._kind = __END__;
	if (!accept_token(tokens, 0)) {
		lexer_error_g.class =
			error_type_to_class(XRE_OUT_OF_MEMORY_ERROR);
		lexer_error_g.type = XRE_OUT_OF_MEMORY_ERROR;

		// assert(false);
		goto lexer_error;
	}

	return (true);

lexer_error:
	lexer_error_g.source = &_token;
	xre_error(&lexer_error_g);

	return (false);
}
