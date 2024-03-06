#include "xre_frame.h"
#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

size_t n_blocks_allocd = 0;
size_t n_arrays_allocd = 0;
size_t n_blocks_freed = 0;
size_t n_arrays_freed = 0;

void allocate_or_die(void **ptr, size_t size) {
  *ptr = malloc(size);
  if (!*ptr) {
    XRE_LOGGER(fatal, "Out of memory");
    exit(EXIT_FAILURE);
  }
}

static bool frame_block_allocate_or_die(frame_block_t **block) {
  __return_val_if_fail__(block, NULL);

  allocate_or_die((void **)block, sizeof(frame_block_t));
  (void)memset(*block, 0, sizeof(frame_block_t));

  n_blocks_allocd++;
  return (true);
}

frame_block_t *error_block_alloc(t_xre_error_type type, t_xre_error_subtype subtype) {
  frame_block_t *block = NULL;

  (void)frame_block_allocate_or_die(&block);
  allocate_or_die((void **)&block->_error, sizeof(t_xre_error));
  
  memset(block->_error, 0, sizeof(t_xre_error));
  block->_error->error.type = type;
  block->_error->error.subtype = subtype;

  return (block);
}

frame_block_t *value_block_alloc(int64_t value) {
  frame_block_t *block = NULL;
  
  (void)frame_block_allocate_or_die(&block);

  block->_type = IF_INTEGER;
  block->_data.value = value;

  return (block);
}

frame_block_t *true_block_alloc(void) {
  frame_block_t *block = value_block_alloc(true);

  block->_type = IF_BOOL;

  return (block);
}

frame_block_t *false_block_alloc(void) {
  frame_block_t *block = value_block_alloc(false);

  block->_type = IF_BOOL;

  return (block);
}

frame_block_t *sequence_block_alloc(array_t *array) {
  frame_block_t *block = NULL;

  (void)frame_block_allocate_or_die(&block);

  block->_type = IF_SEQUENCE;
  block->_data.array = array;

  return (block);
}

frame_block_t *copy_block_alloc(frame_block_t *src) {

  if (src->_type == IF_ARRAY) {
    return (array_block_alloc(src->_data.array));
  }

  if (src->_type == IF_STRING) {
    return (string_block_alloc(src->_data.string));
  }

  if (src->_type == IF_INTEGER) {
    return (value_block_alloc(src->_data.value));
  }

  if (src->_type == IF_BOOL) {
    return (src->_data.value
      ? true_block_alloc()
      : false_block_alloc()
    );
  }

  if (src->_type == IF_SEQUENCE) {
   return (sequence_block_alloc(src->_data.array));
  }
  
  XRE_LOGGER(error, "Unknown operand type");
  return (NULL); 
}

frame_block_t *array_block_alloc(array_t *array) {
  frame_block_t *block = NULL;

  (void)frame_block_allocate_or_die(&block);

  block->_type = IF_ARRAY;

  block->_data.array = array_pull(array, 0, -1);
  if (!block->_data.array) {
    XRE_LOGGER(fatal, "Out of memory");
    exit(EXIT_FAILURE);
  }

  n_arrays_allocd++;

  return (block);
}

// frame_block_t *array_block_from_string_alloc(const char *string) {
//   frame_block_t *block = NULL;
//   (void)frame_block_allocate_or_die(&block);

//   array_t *array = array_create(sizeof(char), strlen(string), NULL);
//   if (!array) {
//     XRE_LOGGER(fatal, "Out of memory");
//     exit(EXIT_FAILURE);
//   }

//   n_arrays_allocd++;

//   (void)memcpy(array_uninitialized_data(array), string, strlen(string));
//   (void)array_append_from_capacity(array, strlen(string));

//   block->_type = IF_STRING;
//   block->_data.array = array;

//   //XRE_LOGGER(info, "array_block (string): %d bytes", array_size(array));
//   return (block);
// }

frame_block_t *string_block_alloc(const char *string) {
  frame_block_t *block = NULL;
  (void)frame_block_allocate_or_die(&block);

  block->_type = IF_STRING;
  block->_data.string = string;

  //XRE_LOGGER(info, "array_block (string): %d bytes", array_size(array));
  return (block);
}

static bool is_truthy_value(int64_t value) {
  return (value != 0);
}

static bool is_truthy_string(const char *string) {
  return (strlen(string) != 0);
}

static bool is_truthy_array(const array_t *array) {
  return (is_truthy_value(array_size(array)));
}

bool is_truthy_block(const frame_block_t *block) {
  if (block->_type
    & (
      IF_INTEGER
      | IF_BOOL
    )) {
    return (is_truthy_value(block->_data.value));
  }
  
  if (block->_type
    & (IF_ARRAY
      | IF_SEQUENCE
      | IF_INJECT
    )) {
    return (is_truthy_array(block->_data.array));
  }

  if (block->_type & IF_STRING) {
    return (is_truthy_string(block->_data.string));
  }

  XRE_LOGGER(error, "Unrecognized block type");
  return (false);
}

static void frame_block_show_one(const frame_block_t *block) {
    __return_if_fail__(block);

  if (block->_error != NULL) {
    xre_error(block->_error, &block->_src->token);
  } else {
    if (block->_type & IF_INTEGER) {
#if defined (__linux__)
      printf("%ld\n", block->_data.value);
#else
      printf("%lld\n", block->_data.value);
#endif
      return ;
    }
    
    if (block->_type & IF_BOOL) {
      printf("%s\n", block->_data.value ? "true" : "false");
      return ;
    }

    if (block->_type & IF_STRING) {
      printf("%s\n", block->_data.string);
      return;
    }

    if (block->_type & IF_ARRAY) {
      printf("[array: %zu]\n", array_size(block->_data.array));
      return;
    }

    if (block->_type & IF_SEQUENCE) {
      printf("[sequence: %zu]\n", array_size(block->_data.array));
      return;
    }

    printf("[other]\n");
  }
}

void frame_block_print(const frame_block_t *block) {
  if (block->_type & IF_SEQUENCE) {
    array_t *array = block->_data.array;
    size_t size = array_size(array);
    size_t i = 0;
    while (i < size) {
      printf("%zu.   ", i);
      frame_block_show_one((const frame_block_t*)array_at(array, i));
      i++;
    }
  } else {
    frame_block_show_one(block);
  }
}

void frame_block_free(frame_block_t **block) {
  __return_if_fail__(block);
  __return_if_fail__(*block);
  
  frame_block_t *frame_block = *block;
  if (frame_block->_type & (
    IF_ARRAY
  )) {
    array_kill(frame_block->_data.array);
    n_arrays_freed++;
  }

  if (frame_block->_error) {
    free(frame_block->_error);
    frame_block->_error = NULL;
  }
  
  free(*block);
  n_blocks_freed++;
  *block = NULL;
}

void print_statistics(void) {
  fprintf(stderr, "STATISTICS:\n");
  fprintf(stderr, "    n_blocks_allocd: %zu (%zu freed)\n", n_blocks_allocd, n_blocks_freed);
  fprintf(stderr, "    n_arrays_allocd: %zu (%zu freed)\n", n_arrays_allocd, n_arrays_freed);
}
