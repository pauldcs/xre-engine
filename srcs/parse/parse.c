#include "xre_parse.h"
#include "array.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include <sys/types.h>
#include <string.h>
#include <assert.h>

# define MICROSTACK_SIZE 2048
static const void	*__stack_a[MICROSTACK_SIZE];
static const void	*__stack_b[MICROSTACK_SIZE];
static size_t	__top_a;
static size_t	__top_b;

# define TOP_A_KIND ((t_xre_ast *)__stack_a[__top_a])->kind
# define TOP_B_KIND ((t_xre_ast *)__stack_b[__top_b])->kind

static void
__push_a (t_xre_ast *node) {
  //printf("push A %s\n", expr_kind_to_string(node->kind));
	if (__top_a < MICROSTACK_SIZE - 1)
		__stack_a[++__top_a] = node;
}
static void
__push_b (t_xre_ast *node) {
  //printf("push B %s\n", expr_kind_to_string(node->kind));
	if (__top_b < MICROSTACK_SIZE - 1)
		__stack_b[++__top_b] = node;
}
static void *
__pop_a (void) {
  //printf("pop A\n");
	if (__top_a > 0)
		return ((void *)__stack_a[__top_a--]);
	return ((void *)0);
}
static void *
__pop_b (void) {
  //printf("pop B\n");
	if (__top_b > 0) 
		return ((void *)__stack_b[__top_b--]);
	return ((void *)0);
}

t_xre_ast *
__ast_new_node (t_xre_token *token) {
  t_xre_ast *node = malloc(sizeof(t_xre_ast));
  bzero(node, sizeof(t_xre_ast));
  node->kind = token->_kind;
  node->type = token->_type;
  
  (void)memcpy((void *)&node->token, token, sizeof(t_xre_token));

  if (node->kind == __VAL__)
    node->value = token->_value;
  
  if (node->kind == __STRING_LITERAL__) {
    node->string = strndup(token->_ptr + 1, token->_len - 2);
  }

    if (node->kind == __IDENTIFIER__) {
    node->string = strndup(token->_ptr, token->_len);
  }
    
  return (node);
}

static int
get_expr_precedence (t_xre_expr_kind kind) {

	switch (kind) {
    case __SCOPE_RESOLUTION__:
      return (0);

    case __POW__:
      return (-1);

    case __ANNOTATE__:
      return (-2);

    case __INJECT__:
      return (-3);

		case __MUL__:
		case __DIV__:
		case __MOD__:
      return (-4);

		case __ADD__:
		case __SUB__:
      return (-5);

		case __LSHIFT__:
		case __RSHIFT__:
      return (-6);

		case __LT__:
    case __GT__:
		case __LE__:
    case __GE__:
      return (-7);

		case __EQ__:
		case __NE__:
      return (-8);

    case __BAND__:
      return (-9);

    case __BXOR__:
      return (-10);

    case __BOR__:
      return (-11);

    case __AND__:
    case __NOT__:
    case __DO__:
      return (-12);

    case __OR__:
    case __ELSE__:
      return (-13);
  
	  case __ASSIGN__:
	  case __ADD_ASSIGN__:
	  case __SUB_ASSIGN__:
	  case __DIV_ASSIGN__:
	  case __MOD_ASSIGN__:
	  case __POW_ASSIGN__:
	  case __MUL_ASSIGN__:
    case __LSHIFT_ASSIGN__:
    case __RSHIFT_ASSIGN__:
    case __OR_ASSIGN__:
    case __AND_ASSIGN__:
      return (-14);
    
    case __LOOP__:
      return (-15);

	  case __SEQUENCE_POINT__:
      return (-16);
  
		case __LPAREN__:
    case __RPAREN__:
      return (-1000);

    case __START__:
    case __END__:
      return (0);

    case __VAL__:
    case __STRING_LITERAL__:
    case __IDENTIFIER__:
      break;
	}

  __return_val_if_fail__(false, 0);
  __builtin_unreachable();
}

static void
__make_value_to_b (void) {
  //printf("merging\n");

	t_xre_ast *node = __pop_a();
  assert(node);

  if (node->type & EXPR_OP_TYPE_UNIOP) {
    node->uniop = __pop_b();

  }
  else { 
    node->binop.right = __pop_b();
    node->binop.left = __pop_b();
  }
	
  __push_b(node);
}

t_xre_ast *
xre_expr_parse (array_t *tokens) {
  __return_val_if_fail__(tokens, NULL);

  t_xre_token *token = NULL;
  size_t idx = 0;

	__top_a = 0;
  __top_b = 0;

	memset(__stack_a, 0, MICROSTACK_SIZE * sizeof(void *));
	memset(__stack_b, 0, MICROSTACK_SIZE * sizeof(void *));

	while (true) {
    token = (t_xre_token *)array_unsafe_at(tokens, ++idx);
    if (!token)
      break;
  
    if (token->_kind == __END__)
      break;
  
		switch (token->_kind) {
      case __LPAREN__:
        __push_a(
          __ast_new_node(token)
        );

      break;

      case __NOT__:
        __push_a(
          __ast_new_node(token)
        );

      break;

      case __RPAREN__:
        while (__top_a && (TOP_A_KIND != __LPAREN__))
          __make_value_to_b();
  
        __pop_a();

      break;
      case __VAL__:
      case __STRING_LITERAL__:
      case __IDENTIFIER__:
          __push_b(
            __ast_new_node(token)
          );

      break;
      default:
        while (__top_a
          && (
            get_expr_precedence(TOP_A_KIND)
            >= get_expr_precedence(token->_kind)
          )) {
          __make_value_to_b();
        }

        __push_a(
          __ast_new_node(token)
        );
		}
	}
	while (__top_a)
		__make_value_to_b();
  
	while (__top_b > 1)
		__make_value_to_b();
	
	return ((t_xre_ast *)__stack_b[__top_b]);
}