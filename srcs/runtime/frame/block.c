#include "xre_parse.h"
#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool is_truthy_value(int64_t value) {
  return (value != 0);
}

static bool is_truthy_string(const char *string) {
  return (strlen(string) != 0);
}

static bool is_truthy_array(const array_t *array) {
  return (is_truthy_value(array_size(array)));
}

bool is_truthy_state(xre_ast_t *node) {

  if (node->event.type == STATE_NUMBER) {
    return (is_truthy_value(node->event.value));
  }
  
  if (node->event.type == STATE_ARRAY) {
    return (is_truthy_array(node->event.array));
  }

  if (node->event.type == STATE_STRING) {
    return (is_truthy_string(node->event.string));
  }

  XRE_LOGGER(error, "Unrecognized block type");
  return (set_error(node, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}

static void state_print_one(state_t state) {
  if (state.type == STATE_NUMBER) {
#if defined (__linux__)
    printf("%ld\n", state.value);
#else
    printf("%lld\n", state.value);
#endif
    return ;
  }

  if (state.type == STATE_STRING) {
    printf("%s\n", state.string);
    return;
  }

  if (state.type == STATE_ARRAY) {
    printf("[array: %zu]\n", array_size(state.array));
    return;
  }

  printf("[other]\n");
 }

void state_print(xre_ast_t *node) {
  if (node->event.type == STATE_ARRAY) {
    array_t *array = node->event.array;
    size_t size = array_size(array);
    size_t i = 0;
    while (i < size) {
      printf("%zu.   ", i);
      state_print_one(*(state_t *)array_at(array, i));
      i++;
    }
  } else {
    state_print_one(node->event);
  }
}

// void frame_block_free(frame_block_t **block) {
//   __return_if_fail__(block);
//   __return_if_fail__(*block);
  
//   frame_block_t *frame_block = *block;
//   if (frame_block->_type & IF_ARRAY) {
//     //array_kill(frame_block->_data.array);
//     frame_block->_data.array = NULL;
//   }

//   if (frame_block->_error) {
//     free(frame_block->_error);
//     frame_block->_error = NULL;
//   }

//   free(*block);
//   *block = NULL;
// }

