#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool bitwise_and_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (state_value(frame, left->state.value & right->state.value));
}

bool bitwise_or_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (state_value(frame, left->state.value | right->state.value));
}

bool bitwise_xor_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  return (state_value(frame, left->state.value ^ right->state.value));
}

bool bitwise_lshift_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value < 0) {
    __return_error(frame, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->state.value > 64) {
    __return_error(frame, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (state_value(frame, left->state.value << right->state.value));
}

bool bitwise_rshift_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (right->state.value < 0) {
    __return_error(frame, XRE_NEGATIVE_SHIFT_ERROR);
  }

  if (right->state.value > 64) {
    __return_error(frame, XRE_EXCEEDS_SHIFT_LIMIT_ERROR);
  }

  return (state_value(frame, left->state.value >> right->state.value));
}

bool bitwise_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  if (left->state.type != STATE_NUMBER) {
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  if (left->state.type != right->state.type) {
    __return_error(frame, XRE_TYPE_MISSMATCH_ERROR);
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

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
