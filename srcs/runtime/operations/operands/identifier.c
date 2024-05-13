#include "xre_assert.h"
#include "xre_log.h"
#include "xre_runtime.h"
#include "xre_utils.h"
#include <string.h>

bool do_identifier(xre_frame_t *frame)
{
	__return_val_if_fail__(frame, NULL);

	symtab_entry_t *item = symtab_get(frame->identifier);
	if (!item) {
		__return_error(frame, XRE_UNBOUND_LOCAL_ERROR);
	}

	xre_state_t *state = &item->state;
	if (IS_FLAG_SET(*state, STATE_ARRAY)) {
		return (state_array_ref(frame, state->array));
	}

	if (IS_FLAG_SET(*state, STATE_STRING)) {
		return (state_string_ref(frame, state->string));
	}

	if (IS_FLAG_SET(*state, STATE_NUMBER)) {
		return (state_value(frame, state->value));
	}

	__return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}

bool identifier_op(xre_frame_t *frame)
{
	__return_val_if_fail__(frame, NULL);

	if (frame->kind == __IDENTIFIER__) {
		return (do_identifier(frame));
	}

	__return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
