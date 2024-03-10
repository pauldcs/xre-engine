#include "array.h"
#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

array_t *runtime_variables = NULL;

bool runtime_variables_init(void) {
  if (!runtime_variables) {
    runtime_variables = array_create(sizeof(stack_item_t), 8, NULL);
    if (!runtime_variables)
      return (false);
  }
  return (true);
}

void runtime_variables_deinit(void) {
  size_t size = array_size(runtime_variables);
  size_t i = 0;
  while (i < size) {
    stack_item_t *item = (stack_item_t *)array_at(runtime_variables, i++);
    state_free(&item->state);
  }
  array_kill(runtime_variables);
  runtime_variables = NULL;
}

stack_item_t *runtime_variables_get(const char *key) {
  size_t size = array_size(runtime_variables);
  size_t i = 0;
  while (i < size) {
    stack_item_t *item = (stack_item_t *)array_at(runtime_variables, i);
    if (strcmp(item->key, key) == 0) {
      return (item);
    }
    i++;
  }
  return (NULL);
}

static bool runtime_variables_add(const char *key, state_t state, varmode_e mode) {
  __return_val_if_fail__(key, false);

  stack_item_t item;
  (void)memset(&item, 0, sizeof(stack_item_t));

  item.key = key;
  item.state = state;
  item.mode = mode;

  if (!array_push(runtime_variables, &item)) {
    XRE_LOGGER(error, "Out of memory");
    exit(1);
  }

  return (true);
}

bool runtime_variables_set(xre_frame_t *frame, const char *key, state_t state, varmode_e mode) {
  __return_val_if_fail__(key, false);
  __return_val_if_fail__(frame, false);
  
  stack_item_t *override = runtime_variables_get(key);
  if (!override) {
    return (runtime_variables_add(key, state, mode));
  }

  if (override->mode == RDONLY) {
  
    log_error_condition_reached;
    return set_error(frame, XRE_ACCESS_VIOLATION, XRE_WRITE_ON_READONLY);
  }

  if (override->mode == STRICT) {
    if (frame->right->state.type != override->state.type) {
    
      log_error_condition_reached;
      return set_error(frame, XRE_ACCESS_VIOLATION, XRE_MODE_WRITE_ON_STRICT);
    }
  }

  if (override->state.type == STATE_ARRAY)
    array_kill(override->state.array);

  (void)memmove(&override->state, &state, sizeof(state_t));

  return (true);
}
