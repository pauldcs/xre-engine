#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

static bool push_frame(array_t *right_array, xre_frame_t *frame)
{
	if (IS_FLAG_SET(frame->state, STATE_ARRAY)) {
		array_t *left_array = frame->state.array;
		size_t size = array_size(left_array);
		size_t i = 0;
		while (i < size) {
			xre_frame_t *frame =
				(xre_frame_t *)array_at(left_array, i);
			if (!array_push(right_array, frame))
				return (false);
			i++;
		}
	} else {
		if (!array_push(right_array, frame))
			return (false);
	}
	return (true);
}

bool sequence_op(xre_frame_t *frame)
{
	__return_val_if_fail__(frame, NULL);

	xre_frame_t *left = LEFT_CHILD(frame);
	xre_frame_t *right = RIGHT_CHILD(frame);

	if (!evaluate(left) || !evaluate(right)) {
		return (false);
	}

	array_t *array = array_create(sizeof(xre_frame_t), 8, NULL);
	if (!array
		|| !push_frame(array, left)
		|| !push_frame(array, right)) {
		__return_error(frame, XRE_OUT_OF_MEMORY_ERROR);
	}

	state_array(frame, array);

	return (true);
}

bool separator_op(xre_frame_t *frame)
{
	__return_val_if_fail__(frame, NULL);

	xre_frame_t *left = LEFT_CHILD(frame);
	xre_frame_t *right = RIGHT_CHILD(frame);

	if (!evaluate(left) || !evaluate(right)) {
		return (false);
	}

	return (state_copy_ref(frame, right));
}
