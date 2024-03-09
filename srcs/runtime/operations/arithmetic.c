#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_log.h"
#include "xre_runtime.h"

// ADD OPERATION
bool add_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  return (change_state_value(node, left->event.value + right->event.value));
}

// SUB OPERATION
bool sub_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  return (change_state_value(node, left->event.value - right->event.value));
}

// MUL OPERATION
bool mul_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  return (change_state_value(node, left->event.value * right->event.value));
}

// DIV OPERATION
bool div_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (right->event.value == 0) {
    return set_error(right, XRE_ARITHMETIC_ERROR, XRE_ZERO_DIVISION_ERROR);
  }

  return (change_state_value(node, left->event.value / right->event.value));
}

// MOD OPERATION
bool mod_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (right->event.value == 0) {
    return set_error(right, XRE_ARITHMETIC_ERROR, XRE_ZERO_DIVISION_ERROR);
  }

  return (change_state_value(node, left->event.value % right->event.value));
}

// ARITHMETIC OPERAITON
bool arithmetic_op(xre_ast_t *node) {
  __return_val_if_fail__(node, NULL);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  if (!evaluate(left)) {
    return (false);
  }

  if (!evaluate(right)) {
    return (false);
  }

  if (left->event.type != STATE_NUMBER) {
    return (set_error(left, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND));
  }

  if (left->event.type != right->event.type) {
    return (set_error(right, XRE_TYPE_ERROR, XRE_TYPE_MISSMATCH_ERROR));
  }

  switch (node->kind) {
  case __ADD__:
  case __ADD_ASSIGN__:
    return (add_op(node));
  case __SUB__:
  case __SUB_ASSIGN__:
    return (sub_op(node));
  case __MUL__:
  case __MUL_ASSIGN__:
    return (mul_op(node));
  case __DIV__:
  case __DIV_ASSIGN__:
    return (div_op(node));
  case __MOD__:
  case __MOD_ASSIGN__:
    return (mod_op(node));
  case __POW__:
  case __POW_ASSIGN__:
  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
