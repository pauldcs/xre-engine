#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool not_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *uni = LEFT_CHILD(frame);

  if (!evaluate(uni)) {
    return (false);
  }

  return (state_value(frame, !is_true_state(uni)));
}
