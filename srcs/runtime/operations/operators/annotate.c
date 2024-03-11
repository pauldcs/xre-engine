#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_frame.h"
#include <stdbool.h>

varmode_t varmodes_tab[] = {
  { RDONLY, "const"},
  { STRICT, "strict"},
};

varmode_e get_mode_by_modestr(const char *string) {
  __return_val_if_fail__(string, NONE);

  size_t i = 0;
  while (i < (sizeof(varmodes_tab) / sizeof(varmode_t))) {
    if (!strcmp(varmodes_tab[i].key, string)) {
      return (varmodes_tab[i].mode);
    }
    i++;
  }
  return NONE;
}

// ARITHMETIC OPERAITON
bool annotation_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, false);

  xre_frame_t *left = frame->left;

  if (left->kind != __IDENTIFIER__) {
    
    log_error_condition_reached;
    return set_error(frame, XRE_TYPE_ERROR, XRE_INVALID_ANNOTATION_ERROR);
  }

  varmode_e mode = get_mode_by_modestr(left->initial.string);
  if (mode == NONE) {
    log_error_condition_reached;
    return set_error(frame, XRE_TYPE_ERROR, XRE_UNKNOWN_ANNOTATION_ERROR);
  }

  (void)memmove(frame, frame->right, sizeof(xre_frame_t));
  frame->mode = mode;
  return (true);
}
