#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_parse.h"
#include <stdbool.h>

bool do_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_runtime_t *left = frame->left;
  xre_runtime_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
    return (change_state_copy(frame, right));
  }

  return (change_state_copy(frame, left));
}

bool else_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_runtime_t *left = frame->left;
  xre_runtime_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (!is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
    return (change_state_copy(frame, right));
  }

  return (change_state_copy(frame, left));
}

bool and_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_runtime_t *left = frame->left;
  xre_runtime_t *right = frame->right;

  if (!evaluate(left)) {
    return (false);
  }

  if (is_truthy_state(left)) {
    if (!evaluate(right)) {
      return (false);
    }
  }

  return (change_state_value(frame, is_truthy_state(right)));
}

bool or_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_runtime_t *left = frame->left;
  xre_runtime_t *right = frame->right;

  if (!evaluate(left))
    return (false);

  if (is_truthy_state(left)) {
    return (change_state_value(frame, true));
  }
  
  if (!evaluate(right)) {
    return (false);
  }

  return (change_state_value(frame, is_truthy_state(right)));
}

bool logical_op(xre_runtime_t *frame) {
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

  XRE_LOGGER(warning, "Confusing condition");
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
