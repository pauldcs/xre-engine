#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

bool print_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *uni = frame->left;

  if (!evaluate(uni)) {
    return (false);
  }

  state_print(uni);

  return (state_copy_ref(frame, uni));
}
