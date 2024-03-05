#include "xre_frame.h"
#include "xre_runtime.h"
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

frame_block_t *runtime_stack_get(const char *key) {
  size_t size = array_size(runtime_stack);
  size_t i = 0;
  while (i < size) {
    stack_item_t *item = (stack_item_t *)array_at(runtime_stack, i);
    if (strcmp(item->key, key) == 0)
      return (&item->block);
   i++;
  }
  return (NULL);
}

frame_block_t *runtime_stack_add(const char *key, frame_block_t *block) {
  stack_item_t item;
  (void)memset(&item, 0, sizeof(stack_item_t));
  
  item.key = key;
  item.block = *block;
  
  if (!array_push(runtime_stack, &item)) {
    XRE_LOGGER(error, "Out of memory");
    exit(1);
  }
  return (block);
}

frame_block_t *runtime_stack_set(const char *key, frame_block_t *block) {
  if (block->_error != NULL)
    return (block);
  
  frame_block_t *override = runtime_stack_get(key);
  if (!override) {
    return (NULL);
  }

  if(override->_type & IF_ARRAY)
    array_kill(override->_data.array);

  (void)memmove(override, block, sizeof(frame_block_t));

  return (block);
}
