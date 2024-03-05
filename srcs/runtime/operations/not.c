#include "xre_runtime.h"
#include "xre_assert.h"

frame_block_t* not_op(frame_block_t *block) {
  __return_val_if_fail__(block, NULL);

  if (block->_error) {
    return (block);
  }

  bool is_truthy = is_truthy_block(block);
  
  frame_block_free(&block);
  return (!is_truthy ? true_block_alloc() : false_block_alloc());
}
