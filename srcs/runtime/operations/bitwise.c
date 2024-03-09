#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_parse.h"

// BITWISE AND OPERATION
bool bitwise_and_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  __return_val_if_fail__(
    left
    && right
    && left->event.type == STATE_NUMBER
    && right->event.type == STATE_NUMBER,
  false);

  change_state_value(node, left->event.value & right->event.value);
  return (true);
}

// BITWISE OR OPERATION
bool bitwise_or_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  __return_val_if_fail__(
    left
    && right
    && left->event.type == STATE_NUMBER
    && right->event.type == STATE_NUMBER,
  false);

  return (change_state_value(node, left->event.value | right->event.value));
}

// BITWISE XOR OPERATION
bool bitwise_xor_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  __return_val_if_fail__(
    left
    && right
    && left->event.type == STATE_NUMBER
    && right->event.type == STATE_NUMBER,
  false);

  return (change_state_value(node, left->event.value ^ right->event.value));
}

// BITWISE LSHIFT OPERATION
bool bitwise_lshift_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  __return_val_if_fail__(
    left
    && right
    && left->event.type == STATE_NUMBER
    && right->event.type == STATE_NUMBER,
  false);

  if (right->event.value < 0) {
    return set_error(right, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->event.value > 64) {
    return set_error(right, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (change_state_value(node, left->event.value << right->event.value));
}

// BITWISE RSHIFT OPERATION
bool bitwise_rshift_op(xre_ast_t *node) {
  __return_val_if_fail__(node, false);

  xre_ast_t *left = node->_binop.left;
  xre_ast_t *right = node->_binop.right;

  __return_val_if_fail__(
    left
    && right
    && left->event.type == STATE_NUMBER
    && right->event.type == STATE_NUMBER,
  false);

  if (right->event.value < 0) {
    return set_error(right, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->event.value > 64) {
    return set_error(right, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (change_state_value(node, left->event.value >> right->event.value));
}

// BITWISE OPERAITON
bool bitwise_op(xre_ast_t *node) {
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
  case __BAND__:
  case __AND_ASSIGN__:
    return (bitwise_and_op(node));
  
  case __BOR__:
  case __OR_ASSIGN__:
    return (bitwise_or_op(node));
  
  case __BXOR__:
    return (bitwise_xor_op(node));
  
  case __LSHIFT__:
  case __LSHIFT_ASSIGN__:
    return (bitwise_lshift_op(node));
  
  case __RSHIFT__:
  case __RSHIFT_ASSIGN__:
    return (bitwise_rshift_op(node));
  
  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
