#include "xre_runtime.h"
#include "xre_parse.h"
#include "xre_assert.h"
#include "xre_log.h"
#include "array.h"

array_t *runtime_stack = NULL;

bool runtime_stack_init(void) {
  if (!runtime_stack) {
    runtime_stack = array_create(sizeof(stack_item_t), 8, NULL);
    if (!runtime_stack)
      return (false);
  }
  return (true);
}

void runtime_stack_deinit(void) {
  array_kill(runtime_stack);
  runtime_stack = NULL;
}

state_t *runtime_stack_get(const char *key) {
  size_t size = array_size(runtime_stack);
  size_t i = 0;
  while (i < size) {
    stack_item_t *item = (stack_item_t *)array_at(runtime_stack, i);
    if (strcmp(item->key, key) == 0) {
      return (&item->state);
    }
    i++;
  }
  return (NULL);
}

bool runtime_stack_add(const char *key, state_t state) {
  __return_val_if_fail__(key, false);

  stack_item_t item;
  (void)memset(&item, 0, sizeof(stack_item_t));
  
  item.key = key;
  item.state = state;
  
  if (!array_push(runtime_stack, &item)) {
    XRE_LOGGER(error, "Out of memory");
    exit(1);
  }

  return (true);
}

bool runtime_stack_set(const char *key, state_t state) {
  __return_val_if_fail__(key, false);
  
  state_t *override = runtime_stack_get(key);
  if (!override) {
    return (false);
  }

  if(override->type == STATE_ARRAY)
    array_kill(override->array);

  (void)memmove(override, &state, sizeof(state_t));

  return (true);
}
