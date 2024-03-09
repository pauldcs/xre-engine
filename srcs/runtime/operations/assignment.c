#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_utils.h"
#include "xre_assert.h"
#include "xre_log.h"

bool simple_assignment(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!runtime_stack_set(left->string, right->event)
    && !runtime_stack_add(left->string, right->event)) 
    return (false);

  return (change_state_copy(node, right));
}

bool reassignement(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;

  if (runtime_stack_set(left->string, node->event))
    return (node);
  
  return (set_error(left, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
}

bool assignment_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  
  if (left->kind != __IDENTIFIER__) {
    return set_error(node, XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (!evaluate(right)) {
    return (false);
  }

  if (node->kind == __ASSIGN__) {
    return (simple_assignment(node));
  }

  state_t *state = runtime_stack_get(left->string);

  if (!state) {
    return set_error(right, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR);
  }

  if (state->type != STATE_NUMBER) {
    return set_error(right, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND);
  }

  change_state_value(left, state->value);

  switch (node->kind) {
  case __ADD_ASSIGN__:
    if (!add_op(node))
      return (false);
    break;
  
  case __SUB_ASSIGN__:
    if (!sub_op(node))
      return (false);
    break;
  
  case __MUL_ASSIGN__:
    if (!mul_op(node))
      return (false);
    break;
  
  case __DIV_ASSIGN__:
    if (!div_op(node))
      return (false);
    break;
  
  case __MOD_ASSIGN__:
    if (!mod_op(node))
      return (false);
    break;
  
  case __POW_ASSIGN__:
  default:
    XRE_LOGGER(warning, "Confusing condition");
    return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
  }

  return (reassignement(node));
}
