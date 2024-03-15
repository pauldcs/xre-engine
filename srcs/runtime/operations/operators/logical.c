#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include <stdbool.h>

bool do_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {

    return (false);
  }

  if (is_true_state(left)) {
    if (!evaluate(right)) {

      return (false);
    }
    return (change_state_copy(frame, right));
  }

  return (change_state_copy(frame, left));
}

bool else_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (!is_true_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
    return (change_state_copy(frame, right));
  }

  return (change_state_copy(frame, left));
}

bool and_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_true_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
  }

  return (change_state_value(frame, is_true_state(right)));
}

bool or_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_true_state(left)) {
    return (change_state_value(frame, true));
  }

  if (!evaluate(right)) {
    return (false);
  }

  return (change_state_value(frame, is_true_state(right)));
}

bool logical_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __DO__:
    return (do_op(frame));

  case __AND__:
    return (and_op(frame));

  case __ELSE__:
    return (else_op(frame));

  case __OR__:
    return (or_op(frame));

  default:
    break;
  }

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
