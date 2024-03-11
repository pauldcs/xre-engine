#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_frame.h"
#include "xre_inject.h"
#include "xre_runtime.h"

injectee_t injectee_tab[] = {
  { "fmt", inject_write},
};

bool run_func_by_id(const char *identifier, xre_frame_t *frame) {
  __return_val_if_fail__(identifier, NONE);
  __return_val_if_fail__(frame, NONE);

  size_t i = 0;
  while (i < (sizeof(injectee_tab) / sizeof(injectee_t))) {
    if (!strcmp(injectee_tab[i].name, identifier)) {
      return (injectee_tab[i].func(frame));
    }
    i++;
  }
  return NONE;
}

bool inject_op(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  xre_frame_t *left = frame->left;
  xre_frame_t *right = frame->right;

  if (left->kind != __IDENTIFIER__) {
  
    log_error_condition_reached;
    return set_error(frame, XRE_TYPE_ERROR, XRE_INVALID_TYPE_FOR_OPERAND);
  }

  if (!evaluate(right)) {

    log_error_return;
    return (false);
  }


  if (!run_func_by_id(left->initial.string, right)) {

    return set_error(frame, XRE_RUNTIME_ERROR, XRE_UNKNOWN_COMMAND_ERROR);
  }

  return (change_state_value(frame, 1));
}
