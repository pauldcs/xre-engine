#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool array_operand(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
