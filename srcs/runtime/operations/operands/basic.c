#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool basic_operand(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  switch (frame->kind) {
  case __VAL__:
    return change_state_value(frame, frame->initial.value);

  case __STRING_LITERAL__:
    return change_state_string(frame, strdup(frame->initial.string));
  default:
    break;
  }

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
