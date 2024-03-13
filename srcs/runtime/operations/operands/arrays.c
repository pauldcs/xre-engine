#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool array_operand(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  log_error_condition_reached;
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
