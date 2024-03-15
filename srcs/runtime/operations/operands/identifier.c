#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool do_identifier(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

    if (!strcmp(frame->initial.string, "exit")) {
    
      __return_error(frame, XRE_EXIT_CALLED_ERROR);
    }

    symtab_entry_t *item = symtab_get(frame->initial.string);
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

      __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
    }

    __return_error(frame, XRE_UNBOUND_LOCAL_ERROR);

}

bool identifier_operand(xre_frame_t *frame) {
  __return_val_if_fail__(frame, NULL);

  if (frame->kind == __IDENTIFIER__) {
    return (do_identifier(frame));
  }

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
