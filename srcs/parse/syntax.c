#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_parse.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#define PREV_TOKEN_KIND ((t_xre_token *)array_at(tokens, idx - 2))->_kind
#define PREV_TOKEN_TYPE ((t_xre_token *)array_at(tokens, idx - 2))->_type

t_xre_error syntax_error;

bool xre_expr_syntax(array_t *tokens) {
  __return_val_if_fail__(tokens, false);

  t_xre_token *token = NULL;
  size_t size = tokens->_nmemb;
  size_t idx = 0;
  int open = 0;

  (void)memset(&syntax_error, 0, sizeof(syntax_error));

  while (size--) {
    token = (t_xre_token *)array_at(tokens, idx++);
    if (!token)
      goto prison;

    switch (token->_kind) {
    case __START__:
      continue;

    case __LPAREN__:
      open++;
#if defined(__linux__)
      __attribute__ ((fallthrough));
#endif
    case __VAL__:
    case __STRING_LITERAL__:
    case __IDENTIFIER__:

      if (PREV_TOKEN_TYPE & (EXPR_OP_TYPE_BINOP | EXPR_OP_TYPE_UNIOP |
                             EXPR_TYPE_CONDITION | EXPR_TYPE_LOOP | EXPR_TYPE_SEQUENCE | EXPR_TYPE_INJECT) ||
          PREV_TOKEN_KIND == __LPAREN__ || PREV_TOKEN_KIND == __START__)
        continue;

      syntax_error.error.type = XRE_SYNTAX_ERROR;
      syntax_error.error.subtype = XRE_UNEXPECTED_OPERAND;

      // assert(false);
      goto syntax_error;
      
    case __ASSIGN__:
    case __ADD_ASSIGN__:
    case __SUB_ASSIGN__:
    case __MUL_ASSIGN__:
    case __DIV_ASSIGN__:
    case __MOD_ASSIGN__:
    case __POW_ASSIGN__:
    case __OR_ASSIGN__:
    case __AND_ASSIGN__:
    case __LSHIFT_ASSIGN__:
    case __RSHIFT_ASSIGN__:

      if (PREV_TOKEN_KIND == __IDENTIFIER__)
        continue;

      syntax_error.error.type = XRE_SYNTAX_ERROR;
      syntax_error.error.subtype = XRE_INVALID_ASSIGMENT_ERROR;

      //assert(false);
      goto syntax_error;
      
    case __NOT__:
      if (PREV_TOKEN_TYPE & (EXPR_OP_TYPE_BINOP | EXPR_OP_TYPE_UNIOP |
                             EXPR_TYPE_CONDITION | EXPR_TYPE_LOOP | EXPR_TYPE_SEQUENCE | EXPR_TYPE_INJECT) ||
          PREV_TOKEN_KIND == __LPAREN__ || PREV_TOKEN_KIND == __START__)
        continue;

      syntax_error.error.type = XRE_SYNTAX_ERROR;
      syntax_error.error.subtype = XRE_UNEXPECTED_OPERATOR;

      // assert(false);
      goto syntax_error;

    case __RPAREN__:
      if (open == 0) {

        syntax_error.error.type = XRE_SYNTAX_ERROR;
        syntax_error.error.subtype = XRE_UNMATCHED_PARENTHESIS;

        // assert(false);
        goto syntax_error;
      }
      open--;
#if defined(__linux__)
      __attribute__ ((fallthrough));
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
    case __INJECT__:
    case __LOOP__:
    case __ANNOTATE__:
    case __SCOPE_RESOLUTION__:
    case __END__:

      if (PREV_TOKEN_TYPE & EXPR_TYPE_OPERAND || PREV_TOKEN_KIND == __RPAREN__)
        continue;

      syntax_error.error.type = XRE_SYNTAX_ERROR;
      syntax_error.error.subtype = XRE_UNEXPECTED_OPERATOR;

      // assert(false);
      goto syntax_error;
    }

    __return_val_if_fail__(false, false);
  }

  if (open) {
    syntax_error.error.type = XRE_SYNTAX_ERROR;
    syntax_error.error.subtype = XRE_UNMATCHED_PARENTHESIS;

    // assert(false);
    goto syntax_error;
  }

  return (true);

syntax_error:
  xre_error(&syntax_error, token);

prison:
  return (false);
}