#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

// ADD OPERATION
bool add_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (change_state_value(frame, left->state.value + right->state.value));
}

// SUB OPERATION
bool sub_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (change_state_value(frame, left->state.value - right->state.value));
}

// MUL OPERATION
bool mul_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (change_state_value(frame, left->state.value * right->state.value));
}

// DIV OPERATION
bool div_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value == 0) {
    __return_error(frame, XRE_ZERO_DIVISION_ERROR);
  }

  return (change_state_value(frame, left->state.value / right->state.value));
}

// MOD OPERATION
bool mod_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value == 0) {
    __return_error(frame, XRE_ZERO_DIVISION_ERROR);
  }

  return (change_state_value(frame, left->state.value % right->state.value));
}

// ARITHMETIC OPERAITON
bool arithmetic_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (!evaluate(right)) {
    return (false);
  }

  if (left->state.type != STATE_NUMBER) {
  
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  if (left->state.type != right->state.type) {
  
    __return_error(frame, XRE_TYPE_MISSMATCH_ERROR);
  }

  switch (frame->kind) {
  case __ADD__:
  case __ADD_ASSIGN__:
    return (add_op(frame));
  
  case __SUB__:
  case __SUB_ASSIGN__:
    return (sub_op(frame));
  
  case __MUL__:
  case __MUL_ASSIGN__:
    return (mul_op(frame));
  
  case __DIV__:
  case __DIV_ASSIGN__:
    return (div_op(frame));
  
  case __MOD__:
  case __MOD_ASSIGN__:
    return (mod_op(frame));
  
  case __POW__:
  case __POW_ASSIGN__:

  default:
    break;
  }

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
