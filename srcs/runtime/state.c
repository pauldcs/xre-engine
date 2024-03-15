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

xre_frame_t *state_init(xre_ast_t *ast) {
  xre_frame_t *frame = malloc(sizeof(xre_frame_t));
  (void)memset(frame, 0, sizeof(xre_frame_t));

  switch (ast->kind) {
  case __VAL__:
    frame->initial.value = ast->value;
    break;

  case __STRING_LITERAL__:
  case __IDENTIFIER__:
    frame->initial.string = strdup(ast->string);
    break;

  case __NOT__:
  case __PRINT__:
    frame->left = state_init(ast->uniop);
    break;
  
  default:
    frame->left = state_init(ast->_binop.left);
    frame->right = state_init(ast->_binop.right);
    break;
  }

  frame->kind = ast->kind;
  frame->type = ast->type;
  frame->token = (xre_token_t *)&ast->token;

  frame->state.type = STATE_UNDEFINED;
  return (frame);
}

void state_free(state_t *state) {
  switch (state->type) {
  case STATE_STRING:
    free((char *)state->string);
    break;

  case STATE_ARRAY:
    array_kill(state->array);
    break;
  
  default:
    break;
  }
}

void state_deinit(xre_frame_t *frame) {
  switch (frame->kind) {
  case __STRING_LITERAL__:
  case __IDENTIFIER__:
    free((void *)frame->initial.string);
    break;
  
  case __VAL__:
      break;

  default:
    if (frame->left) state_deinit(frame->left);
    if (frame->right) state_deinit(frame->right);         
  }
  state_free(&frame->state);
  free(frame);
}

bool change_state_value(xre_frame_t *frame, int64_t value) {
  __return_val_if_fail__(frame, false);
  
  state_free(&frame->state);

  frame->state.type = STATE_NUMBER;
  frame->state.value = value;
  return (true);
}

bool change_state_string(xre_frame_t *frame, char *string) {
  __return_val_if_fail__(frame, false);
  __return_val_if_fail__(string, false);

  state_free(&frame->state);

  frame->state.type = STATE_STRING;
  frame->state.string = string;
  return (true);
}

bool change_state_array(xre_frame_t *frame, array_t *array) {
  __return_val_if_fail__(frame, false);
  __return_val_if_fail__(array, false);


  frame->state.type = STATE_ARRAY;
  frame->state.array = array;
  return (true);
}

bool change_state_copy(xre_frame_t *this, xre_frame_t *that) {
  __return_val_if_fail__(this, false);
  __return_val_if_fail__(that, false);

  state_free(&this->state);

  (void)memcpy(&this->state, &that->state, sizeof(state_t));

  if (that->state.type == STATE_ARRAY) {
    this->state.array = array_pull(that->state.array, 0, -1);
  }

  if (that->state.type == STATE_STRING) {
    this->state.string = strdup(that->state.string);
  }

  return (true);
}

static bool is_truthy_value(int64_t value) { return (value != 0); }

static bool is_truthy_string(const char *string) {
  return (strlen(string) != 0);
}

static bool is_truthy_array(const array_t *array) {
  return (is_truthy_value(array_size(array)));
}

bool is_true_state(xre_frame_t *frame) {

  if (frame->state.type == STATE_NUMBER) {
    return (is_truthy_value(frame->state.value));
  }

  if (frame->state.type == STATE_ARRAY) {
    return (is_truthy_array(frame->state.array));
  }

  if (frame->state.type == STATE_STRING) {
    return (is_truthy_string(frame->state.string));
  }

  __return_error(frame, XRE_UNDEFINED_BEHAVIOR_ERROR);
}

const char *state_to_str(state_t *state) {

  if (state->type == STATE_NUMBER) {
    return ("number");
  }

  if (state->type == STATE_ARRAY) {
    return ("array");
  }

  if (state->type == STATE_STRING) {
    return ("array");
  }

  return ("unknown");
}

static void state_print_one(state_t state) {
  if (state.type == STATE_NUMBER) {
#if defined(__linux__)
    printf("%ld\n", state.value);
#else
    printf("%lld\n", state.value);
#endif
    return;
  }

  if (state.type == STATE_STRING) {
    printf("%s\n", state.string);
    return;
  }

  if (state.type == STATE_ARRAY) {
    printf("[array %p: %zu]\n", (void*)state.array,  array_size(state.array));
    return;
  }

  printf("[undefined]\n");
}

void state_print(xre_frame_t *frame) {
  if (frame->state.type == STATE_ARRAY) {
    array_t *array = frame->state.array;
    size_t size = array_size(array);
    size_t i = 0;
    while (i < size) {
      printf("%zu.   ", i);
      xre_frame_t *frame = (xre_frame_t *)array_at(array, i);
      state_t state = frame->state;
      state_print_one(state);
      i++;
    }
  } else {
    state_print_one(frame->state);
  }
}

static void
__state_debug (xre_frame_t *frame, size_t depth) {
	size_t	i;

	i = 0;
	while (i++ < depth)
		write(1, "   ", 3);

  if (!frame) {
    printf("!NULL!\n");
		return ;
	}

  state_print_one(frame->state);

	if (frame->left)
		__state_debug(frame->left, depth + 1);
  if (frame->left)
		__state_debug(frame->right, depth + 1);
}

void
state_debug (xre_frame_t *frame) {
	__state_debug(frame, 0);
}
