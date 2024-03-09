#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_parse.h"

bool inject_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_runtime_t *left = frame->left;
  xre_runtime_t *right = frame->right;

  if (left->kind != __IDENTIFIER__) {
    return set_error(frame, XRE_TYPE_ERROR, XRE_INVALID_ASSIGMENT_ERROR);
  }

  if (!evaluate(right)) {
    return (false);
  }

  state_print(right);

  bool is_truthy = is_truthy_state(right);

  return (change_state_value(frame, is_truthy));
}
