#include "xre_parse.h"
#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_log.h"

xre_frame_t *frame_tree_g = NULL;

static bool alloc_frame_tree(size_t size) {
  frame_tree_g = malloc(sizeof(xre_frame_t) * size);
  if (!frame_tree_g) {
    XRE_LOGGER(error, "Out of memory");
    return (false);
  }
  (void)memset(frame_tree_g, 0x00, sizeof(xre_frame_t) * size);
  return (true);
}

static int frame_tree_create(xre_ast_t *ast) {
  static int index = 0;
  xre_frame_t *frame = &frame_tree_g[index];
  int initial_index = index++;

  switch (ast->kind) {
  case __VAL__:
    frame->state.attrs = STATE_NUMBER;
    frame->state.value = ast->value;
    frame->left_index = -1;
    frame->right_index = -1;
    break;

  case __STRING_LITERAL__:
    frame->state.attrs = STATE_STRING;
    frame->state.string = (char *)ast->string;
    frame->state.attrs |= REFERENCE_FLAG;
    frame->left_index = -1;
    frame->right_index = -1;
    break;
  
  case __IDENTIFIER__:
    frame->state.attrs = STATE_UNDEFINED;
    frame->identifier = (char *)ast->string;
    frame->state.attrs |= REFERENCE_FLAG;
    frame->left_index = -1;
    frame->right_index = -1;
    break;

  case __NOT__:
  case __PRINT__:
    frame->state.attrs = STATE_UNDEFINED;
    frame->left_index = frame_tree_create(ast->uniop);
    frame->right_index = -1;
    break;
  
  default:
    frame->state.attrs = STATE_UNDEFINED;
    frame->left_index = frame_tree_create(ast->_binop.left);
    frame->right_index = frame_tree_create(ast->_binop.right);
    break;
  }

  frame->kind = ast->kind;
  frame->token = (xre_token_t *)&ast->token;

  return (initial_index);
}

static size_t get_ast_size(xre_ast_t *ast) {
  size_t size = 1;
  
  switch (ast->kind) {
  case __VAL__:
  case __STRING_LITERAL__:
  case __IDENTIFIER__:
    return (size);

  case __NOT__:
  case __PRINT__:
    return (size
      + get_ast_size(ast->_binop.left));
  
  default:
    return (size
      + get_ast_size(ast->_binop.left)
      + get_ast_size(ast->_binop.right));
  }
}

int init_frame_tree(xre_ast_t *ast) {
  __return_val_if_fail__(ast, -1);

  if (!alloc_frame_tree(get_ast_size(ast)))
    return (-1);
  
  return (frame_tree_create(ast));
}

void state_free(xre_state_t *state) {
  if (IS_FLAG_SET(*state, STATE_ARRAY)) {
    array_kill(state->array);
  }  
}

void deinit_frame_tree(xre_frame_t *frame) {
  if (HAS_LEFT_CHILD(frame)) {
    deinit_frame_tree(LEFT_CHILD(frame));
  }

  if (HAS_RIGHT_CHILD(frame)) {
    deinit_frame_tree(RIGHT_CHILD(frame));
  }         
  
  if (!IS_REF_STATE(frame->state)) {
    state_free(&frame->state);
  }
}
