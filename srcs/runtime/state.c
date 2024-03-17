#include "array.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool state_value(xre_frame_t *frame, int64_t value)
{
	__return_val_if_fail__(frame, false);

	frame->state.attrs = STATE_NUMBER;
	frame->state.value = value;
	return (true);
}

bool state_string(xre_frame_t *frame, char *string)
{
	__return_val_if_fail__(frame, false);
	__return_val_if_fail__(string, false);

	frame->state.attrs = STATE_STRING;
	frame->state.string = string;

	return (true);
}

bool state_array(xre_frame_t *frame, array_t *array)
{
	__return_val_if_fail__(frame, false);
	__return_val_if_fail__(array, false);

	frame->state.attrs = STATE_ARRAY;
	frame->state.array = array;

	return (true);
}

bool state_string_ref(xre_frame_t *frame, char *string)
{
	(void)state_string(frame, string);
	ADD_FLAG(frame->state, REFERENCE_FLAG);

	return (true);
}

bool state_array_ref(xre_frame_t *frame, array_t *array)
{
	(void)state_array(frame, array);
	ADD_FLAG(frame->state, REFERENCE_FLAG);

	return (true);
}

bool state_copy_ref(xre_frame_t *this, xre_frame_t *that)
{
	__return_val_if_fail__(this, false);
	__return_val_if_fail__(that, false);

	if (IS_FLAG_SET(that->state, STATE_ARRAY)) {
		return (state_array_ref(this, that->state.array));
	}

	if (IS_FLAG_SET(that->state, STATE_STRING)) {
		return (state_string_ref(this, that->state.string));
	}

	if (IS_FLAG_SET(that->state, STATE_NUMBER)) {
		return (state_value(this, that->state.value));
	}

	__return_error(this, XRE_UNDEFINED_BEHAVIOR_ERROR);
}

static bool is_truthy_value(int64_t value)
{
	return (value != 0);
}

static bool is_truthy_string(const char *string)
{
	return (strlen(string) != 0);
}

static bool is_truthy_array(const array_t *array)
{
	return (is_truthy_value(array_size(array)));
}

bool is_true_state(xre_frame_t *frame)
{
	if (IS_FLAG_SET(frame->state, STATE_NUMBER)) {
		return (is_truthy_value(frame->state.value));
	}

	if (IS_FLAG_SET(frame->state, STATE_ARRAY)) {
		return (is_truthy_array(frame->state.array));
	}

	if (IS_FLAG_SET(frame->state, STATE_STRING)) {
		return (is_truthy_string(frame->state.string));
	}

	__return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}

const char *state_to_str(xre_state_t *state)
{
	if (IS_FLAG_SET(*state, STATE_NUMBER)) {
		return ("number");
	}

	if (IS_FLAG_SET(*state, STATE_ARRAY)) {
		return ("array");
	}

	if (IS_FLAG_SET(*state, STATE_STRING)) {
		return ("array");
	}

	return ("unknown");
}

static void state_print_one(xre_state_t state)
{
	if (IS_FLAG_SET(state, STATE_NUMBER)) {
#if defined(__linux__)
		printf("%ld\n", state.value);
#else
		printf("%lld\n", state.value);
#endif
		return;
	}

	if (IS_FLAG_SET(state, STATE_STRING)) {
		printf("%s\n", state.string);
		return;
	}

	if (IS_FLAG_SET(state, STATE_ARRAY)) {
		printf("[array %p: %zu]\n", (void *)state.array,
		       array_size(state.array));
		return;
	}

	printf("[undefined]\n");
}

void state_print(xre_frame_t *frame)
{
	if (IS_FLAG_SET(frame->state, STATE_ARRAY)) {
		array_t *array = frame->state.array;
		size_t size = array_size(array);
		size_t i = 0;
		while (i < size) {
			printf("%zu.   ", i);
			xre_frame_t *frame = (xre_frame_t *)array_at(array, i);
			xre_state_t state = frame->state;
			state_print_one(state);
			i++;
		}
	} else {
		state_print_one(frame->state);
	}
}

static void __state_debug(xre_frame_t *frame, size_t depth)
{
	size_t i;

	i = 0;
	while (i++ < depth)
		write(1, "   ", 3);

	if (!frame) {
		printf("!NULL!\n");
		return;
	}

	state_print_one(frame->state);
	// printf("%s\n", frame->is_ref ? "ref" : "owned");

	if (HAS_LEFT_CHILD(frame)) {
		__state_debug(LEFT_CHILD(frame), depth + 1);
	}

	if (HAS_RIGHT_CHILD(frame)) {
		__state_debug(RIGHT_CHILD(frame), depth + 1);
	}
}

void state_debug(xre_frame_t *frame)
{
	__state_debug(frame, 0);
}
