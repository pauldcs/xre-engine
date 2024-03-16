#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool bitwise_and_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

  return (state_value(frame, left->state.value & right->state.value));
}

bool bitwise_or_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

  return (state_value(frame, left->state.value | right->state.value));
}

bool bitwise_xor_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

  return (state_value(frame, left->state.value ^ right->state.value));
}

bool bitwise_lshift_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

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

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

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

  xre_frame_t *left = LEFT_CHILD(frame);
  xre_frame_t *right = RIGHT_CHILD(frame);

  if (!evaluate(left) || !evaluate(right)) {
    return (false);
  }

  if (!IS_FLAG_SET(left->state, STATE_NUMBER)) {
    __return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
  }

  if (COMPARE_FLAGS(left->state, right->state)) {
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
