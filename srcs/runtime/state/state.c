#include "xre_parse.h"
#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_errors.h"
#include "xre_log.h"
#include "array.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

xre_runtime_t *init_state(xre_ast_t *ast) {
    xre_runtime_t *frame = malloc(sizeof(xre_runtime_t));
    (void)memset(frame, 0, sizeof(xre_runtime_t)); 
  
    switch(ast->kind) {
      case __VAL__:
        frame->initial.value = ast->value;
        break;
      
      case __STRING_LITERAL__:
      case __IDENTIFIER__:
        frame->initial.string = strdup(ast->string);
        break;

      case __NOT__:
        frame->left = init_state(ast->uniop);
        break;
   
      default:
        frame->left = init_state(ast->_binop.left);
        frame->right = init_state(ast->_binop.right); 
        break;         
    }

    frame->kind = ast->kind; 
    frame->type = ast->type;
    frame->token = (xre_token_t *)&ast->token;

    frame->state.type = STATE_UNDEFINED;
    return (frame);
}

void free_state(state_t *state) {
  switch (state->type) {
    case STATE_STRING:
      free(state->string);
      break;
    
    case STATE_ARRAY:
      array_kill(state->array);
      break;
    
    default:
      break;
  }
}

void deinit_state(xre_runtime_t *frame) {
    switch(frame->kind) {
      case __VAL__:
        break;
      
      case __STRING_LITERAL__:
      case __IDENTIFIER__:
        free((char*)frame->initial.string);
        break;

      case __NOT__:
        deinit_state(frame->left);
        break;
   
      default:
        deinit_state(frame->left);
        deinit_state(frame->right); 
        break;         
    }
  
    free_state(&frame->state);
    free(frame);
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

bool is_truthy_state(xre_runtime_t *frame) {

  if (frame->state.type == STATE_NUMBER) {
    return (is_truthy_value(frame->state.value));
  }
  
  if (frame->state.type == STATE_ARRAY) {
    return (is_truthy_array(frame->state.array));
  }

  if (frame->state.type == STATE_STRING) {
    return (is_truthy_string(frame->state.string));
  }

  XRE_LOGGER(error, "Unrecognized block type");
  return (set_error(frame, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
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

void state_print(xre_runtime_t *frame) {
  if (frame->state.type == STATE_ARRAY) {
    array_t *array = frame->state.array;
    size_t size = array_size(array);
    size_t i = 0;
    while (i < size) {
      printf("%zu.   ", i);
      state_print_one(*(state_t *)array_at(array, i));
      i++;
    }
  } else {
    state_print_one(frame->state);
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

