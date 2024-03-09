#include "xre_runtime.h"
#include "array.h"
#include "xre_args.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_stringf.h"
#include "xre_utils.h"
#include <iso646.h>
#include <stdbool.h>
#include <sys/types.h>

bool        _has_error = false;
t_xre_error _error;

bool set_error(xre_ast_t*node, t_xre_error_type type, t_xre_error_subtype subtype) {
  _error.error.type = type;
  _error.error.subtype = subtype;
  _error.src = &node->token;
  _has_error = true;
  return (false);
}

bool error_occurred(void) {
  return (_has_error);
}

bool change_state_value(xre_ast_t *node, int64_t value) {
  node->event.type = STATE_NUMBER;
  node->event.value = value;
  return (true);
}

bool change_state_array(xre_ast_t *node, array_t *array) {
  node->event.type = STATE_ARRAY;
  node->event.array = array;
  return (true);
}

bool change_state_string(xre_ast_t *node, const char *string) {
  node->event.type = STATE_STRING;
  node->event.string = string;
  return (true);
}

bool change_state_copy(xre_ast_t *this, xre_ast_t *that) {
  (void)memcpy(&this->event, &that->event, sizeof(state_t));
  return (true);
}

bool binop_exec(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  switch (node->kind) {
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
    return (assignment_op(node));

  case __ADD__:
  case __SUB__:
  case __MUL__:
  case __DIV__:
  case __MOD__:
  case __POW__:
    return (arithmetic_op(node));

  case __EQ__:
  case __NE__:
  case __LT__:
  case __GT__:
  case __LE__:
  case __GE__:
    return (relational_op(node));

  case __BAND__:
  case __BOR__:
  case __BXOR__:
  case __LSHIFT__:
  case __RSHIFT__:
    return (bitwise_op(node));
  
  case __LOOP__:
    return (loop_op(node));

  case __SEQUENCE_POINT__:
    return (sequence_op(node));
  
  case __SEPARATOR__:
    return (separator_op(node));
  
  case __INJECT__:
    return (inject_op(node));
  
  case __DO__:
  case __ELSE__:
  case __AND__:
  case __OR__:
    return (logical_op(node));
  
  default:
    break;
  }

  return (set_error(node, XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
}

bool uniop_exec(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  switch (node->kind) {
  case __NOT__:
    return (not_op(node));
    
  default:
    return (set_error(node, XRE_INTERNAL_ERROR, XRE_NOT_IMPLEMENTED_ERROR));
  }
}

bool operand_exec(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  return (operand(node));
}


bool evaluate(xre_ast_t *ast) {
  __return_val_if_fail__(ast, NULL);

  switch (ast->type) {
  case EXPR_TYPE_VALUE:
    return operand_exec(ast);

  case EXPR_OP_TYPE_BINOP:
    return binop_exec(ast);

  case EXPR_OP_TYPE_UNIOP:
    return uniop_exec(ast);

  case EXPR_TYPE_OTHER:
  default:
    XRE_LOGGER(warning, "Confusing condition");
    return (set_error(ast, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
  }
}

bool xre_runtime(xre_ast_t *ast) {
  __return_val_if_fail__(ast, false);

  if (!runtime_stack_init()) {
    return (false);
  }
  
  if (!evaluate(ast)) {
    if (error_occurred()) { 
      xre_error(&_error, _error.src);
      return (false);
    }
    XRE_LOGGER(error, "Failed without error message");
    return (false);
  }
  
  if (__xre_args__.flags & SHOW_EXPR_RESULT) {
    state_print(ast);
  }
  
  runtime_stack_deinit();

  return (true);
}
