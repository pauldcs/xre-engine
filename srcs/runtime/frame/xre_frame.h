#ifndef __XRE_FRAME_H__
# define __XRE_FRAME_H__

# include "xre_parse.h"
# include "xre_errors.h"
# include <sys/types.h>

extern size_t n_allocs;
extern size_t n_arrays_allocd;
extern size_t n_arrays_freed;
extern size_t n_frees;

typedef struct {
  int64_t          _type;
  t_xre_error     *_error;
  const t_xre_ast *_src;
  union {
    int64_t     value;
    array_t    *array;
    const char *string;
  } _data;
} frame_block_t;


typedef struct stack_item {
  const char   *key;
  frame_block_t block;
} stack_item_t;

extern array_t *runtime_stack;

// FRAME
bool           runtime_stack_init(void);
void           runtime_stack_deinit(void);
frame_block_t *runtime_stack_get(const char *key);
frame_block_t *runtime_stack_add(const char *key, frame_block_t *block);
frame_block_t *runtime_stack_set(const char *key, frame_block_t *block);

// BLOCK
frame_block_t *error_block_alloc(t_xre_error_type type, t_xre_error_subtype subtype);
frame_block_t *value_block_alloc(int64_t value);
frame_block_t *true_block_alloc(void);
frame_block_t *false_block_alloc(void);
frame_block_t *sequence_block_alloc(array_t *array);
frame_block_t *array_block_alloc(array_t *array);
frame_block_t *copy_block_alloc(frame_block_t *src);
frame_block_t *array_block_from_string_alloc(const char *string);
frame_block_t *string_block_alloc(const char *string);
bool           is_truthy_block(const frame_block_t *block);

// COMMON
void frame_block_print(const frame_block_t *block);
void frame_block_free(frame_block_t **block);

void print_statistics(void);

#endif /* __XRE_FRAME_H__ */
