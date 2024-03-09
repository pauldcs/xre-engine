#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_parse.h"

bool not_op(xre_runtime_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_runtime_t *uni = frame->left;

  if (!evaluate(uni)) {
    return (false);
  }

  return (change_state_value(frame, !is_truthy_state(uni)));
}
