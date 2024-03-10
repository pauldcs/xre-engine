#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

// BITWISE AND OPERATION
bool bitwise_and_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  change_state_value(frame, left->state.value & right->state.value);
  return (true);
}

// BITWISE OR OPERATION
bool bitwise_or_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (change_state_value(frame, left->state.value | right->state.value));
}

// BITWISE XOR OPERATION
bool bitwise_xor_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (change_state_value(frame, left->state.value ^ right->state.value));
}

// BITWISE LSHIFT OPERATION
bool bitwise_lshift_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value < 0) {
  
    log_error_condition_reached;
    return set_error(right, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->state.value > 64) {
  
    log_error_condition_reached;
    return set_error(right, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (change_state_value(frame, left->state.value << right->state.value));
}

// BITWISE RSHIFT OPERATION
bool bitwise_rshift_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value < 0) {
  
    log_error_condition_reached;
    return set_error(right, XRE_VALUE_ERROR, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->state.value > 64) {
  
    log_error_condition_reached;
    return set_error(right, XRE_VALUE_ERROR, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (change_state_value(frame, left->state.value >> right->state.value));
}

// BITWISE OPERAITON
bool bitwise_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    
    log_error_return;
    return (false);
  }

  if (!evaluate(right)) {

    log_error_return;
    return (false);
  }

  if (left->state.type != STATE_NUMBER) {
  
    log_error_condition_reached;
    return (set_error(left, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND));
  }

  if (left->state.type != right->state.type) {
  
    log_error_condition_reached;
    return (set_error(right, XRE_TYPE_ERROR, XRE_TYPE_MISSMATCH_ERROR));
  }

  switch (frame->kind) {
  case __BAND__:
    return (bitwise_and_op(frame));

  case __BOR__:
    return (bitwise_or_op(frame));

  case __BXOR__:
    return (bitwise_xor_op(frame));

  case __LSHIFT__:
    return (bitwise_lshift_op(frame));

  case __RSHIFT__:
    return (bitwise_rshift_op(frame));

  default:
    XRE_LOGGER(error, "Unrecognized arithmetic operation");
  }

  log_error_condition_reached;
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}