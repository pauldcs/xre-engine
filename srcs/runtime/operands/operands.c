#include "xre_runtime.h"
#include "xre_assert.h"
#include "xre_utils.h"
#include "xre_log.h"

frame_block_t* operand(t_xre_ast *ast) {
  __return_val_if_fail__(ast, NULL);

  if (ast->kind == __IDENTIFIER__) {
    if (!strcmp(ast->string, "exit")) {
      XRE_LOGGER(info, "exiting ...");
      return (error_block_alloc(XRE_EXIT_ERROR, XRE_EXIT_CALLED_ERROR));
    }
  
    frame_block_t *block = runtime_stack_get(ast->string);
    if (block) {
      if (block->_type == IF_ARRAY) {
        return (array_block_alloc(block->_data.array));
      }
      
      if (block->_type == IF_INTEGER) {
        return (value_block_alloc(block->_data.value));
      }

      if (block->_type == IF_STRING) {
        return (string_block_alloc(block->_data.string));
      }
      
      if (block->_type == IF_BOOL) {
        return (block->_data.value ? true_block_alloc() : false_block_alloc());
      }

      if (block->_type == IF_SEQUENCE) {
       return (sequence_block_alloc(block->_data.array));
      }
    
      XRE_LOGGER(warning, "Confusing condition");
      return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
    }
    
    return (error_block_alloc(XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
  }
  
  switch (ast->kind) {
    case __VAL__:
      return (value_block_alloc(ast->value));
    
    case __STRING_LITERAL__:
      return (string_block_alloc(ast->string));

    default:
      break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (error_block_alloc(XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}

frame_block_t* operand_with(frame_block_t *buffer, t_xre_ast *ast) {
  __return_val_if_fail__(ast, NULL);

  if (ast->kind == __IDENTIFIER__) {
    if (!strcmp(ast->string, "exit")) {
      XRE_LOGGER(info, "exiting ...");
      return (error_block_with(buffer, XRE_EXIT_ERROR, XRE_EXIT_CALLED_ERROR));
    }
  
    frame_block_t *block = runtime_stack_get(ast->string);
    if (block) {
      if (block->_type == IF_ARRAY) {
        return (array_block_alloc(block->_data.array));
      }
      
      if (block->_type == IF_INTEGER) {
        return (value_block_with(buffer, block->_data.value));
      }

      if (block->_type == IF_STRING) {
        return (string_block_with(buffer, block->_data.string));
      }
      
      if (block->_type == IF_BOOL) {
        return (block->_data.value ? true_block_with(buffer) : false_block_with(buffer));
      }

      if (block->_type == IF_SEQUENCE) {
       return (sequence_block_alloc(block->_data.array));
      }
    
      XRE_LOGGER(warning, "Confusing condition");
      return (error_block_with(buffer, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
    }
    
    return (error_block_with(buffer, XRE_RUNTIME_ERROR, XRE_UNBOUND_LOCAL_ERROR));
  }
  
  switch (ast->kind) {
    case __VAL__:
      return (value_block_with(buffer, ast->value));
    
    case __STRING_LITERAL__:
      return (string_block_with(buffer, ast->string));

    default:
      break;
  }

  XRE_LOGGER(warning, "Confusing condition");
  return (error_block_with(buffer, XRE_INTERNAL_ERROR, XRE_CONFUSING_CONDITION));
}
