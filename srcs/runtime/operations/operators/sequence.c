#include "xre_assert.h"
#include "xre_parse.h"
#include "xre_runtime.h"

static void push_frame(array_t *array, xre_frame_t *frame)
{
	if (IS_FLAG_SET(frame->state, STATE_ARRAY)) {
		array_t *array_2 = frame->state.array;
		size_t size = array_size(array_2);
		size_t i = 0;
		while (i < size) {
			xre_frame_t *frame =
				(xre_frame_t *)array_at(array_2, i);
			array_push(array, frame);
			i++;
		}
	} else {
		array_push(array, frame);
	}
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
	push_frame(array, left);
	push_frame(array, right);

	return (state_array(frame, array));
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
