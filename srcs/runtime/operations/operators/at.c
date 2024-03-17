#include "xre_assert.h"
#include "xre_log.h"
#include "xre_operations.h"
#include "xre_runtime.h"
#include <stdbool.h>

bool at_op(xre_frame_t *frame)
{
	__return_val_if_fail__(frame, NULL);

	xre_frame_t *left = LEFT_CHILD(frame);
	xre_frame_t *right = RIGHT_CHILD(frame);

	if (!evaluate(left) || !evaluate(right)) {
		return (false);
	}

	if (!IS_FLAG_SET(left->state, STATE_ARRAY)) {
		__return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
	}

	if (!IS_FLAG_SET(right->state, STATE_NUMBER)) {
		__return_error(frame, XRE_INVALID_TYPE_FOR_OPERAND_ERROR);
	}

	array_t *array = left->state.array;
	size_t value = right->state.value;

	if (value >= array_size(array)) {
		__return_error(frame, XRE_OUT_OF_BOUNDS_ACCESS_ERROR);
	}

	xre_frame_t *at_index = (xre_frame_t *)array_at(array, value);

	if (IS_FLAG_SET(at_index->state, STATE_ARRAY)) {
		return (state_array_ref(frame, at_index->state.array));
	}

	if (IS_FLAG_SET(at_index->state, STATE_STRING)) {
		return (state_string_ref(frame, at_index->state.string));
	}

	if (IS_FLAG_SET(at_index->state, STATE_NUMBER)) {
		return (state_value(frame, at_index->state.value));
	}

	__return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}
