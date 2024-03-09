#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool loop_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  size_t max_iterations = DEFAULT_MAX_ITERATIONS;

  for (;;) {
    if (!max_iterations--) {
      return (set_error(frame, XRE_RUNTIME_ERROR, XRE_NOT_TERMINATED_ERROR));
    }

    if (!evaluate(left)) {
      return (false);
    }

    if (!is_true_state(left)) {
      break;
    }
    // printf("bite1\n");

    if (!evaluate(right)) {
      return (false);
    }

    (void)change_state_copy(frame, right);
  }

  return (change_state_copy(frame, left));
}
