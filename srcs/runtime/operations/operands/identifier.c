#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool do_identifier(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

    if (!strcmp(frame->initial.string, "exit")) {
    
      log_error_condition_reached;
      return (set_error(frame, XRE_EXIT_ERROR, XRE_EXIT_CALLED_ERROR));
    }

    stack_item_t *item = runtime_variables_get(frame->initial.string);
    if (item) {
      state_t *state = &item->state;
      if (state->type == STATE_ARRAY) {
        return (change_state_array(frame, array_pull(state->array, 0, -1)));
      }

      if (state->type == STATE_STRING) {
        return (change_state_string(frame, strdup(state->string)));
      }

      if (state->type == STATE_NUMBER) {
        return (change_state_value(frame, state->value));
      }

      log_error_condition_reached;
      return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
    }

    log_error_condition_reached;
    return (set_error(frame, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));

}

bool identifier_operand(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  if (frame->kind == __IDENTIFIER__) {
    return (do_identifier(frame));
  }

  log_error_condition_reached;
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
