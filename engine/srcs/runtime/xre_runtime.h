#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_parse.h"
#include <stdbool.h>

/*    The parsed AST is tranformed into this 'ast_stmt_t'
 *    which is essencially the same except that the tree is
 *    in an array.
 */

typedef struct stmt_tree_s ast_stmt_t;
typedef int stmt_t;
typedef bool (*runtime_op_t)(ast_stmt_t *);

extern ast_stmt_t *__global_current_stmts_ptr__;

struct stmt_tree_s {
	runtime_op_t eval;
	xre_token_t *
		orig; // the code fragment from which this statement is parsed from
	union {
		int64_t value; // the value that the token represents
		char *string; // the string that the token represents
		struct {
			stmt_t left; // index of the left child branch
			stmt_t right; // index of the right child branch
		} br; // the operator that the token represents
	};
};

#define __br_eval(obj_ptr) obj_ptr->eval(obj_ptr)

#define __push_r_as_ref(self_ptr, obj_ptr)         \
	stack_push_enable_attrs(self_ptr, obj_ptr, \
				ATTR_READABLE | ATTR_REFERENCE)

#define __push_r(self_ptr, obj_ptr) \
	stack_push_enable_attrs(self_ptr, obj_ptr, ATTR_READABLE)

#define __push_w(self_ptr, obj_ptr) \
	stack_push_enable_attrs(self_ptr, obj_ptr, ATTR_MUTABLE)

#define __push_rw(self_ptr, obj_ptr) \
	stack_push_enable_attrs(self_ptr, obj_ptr, ATTR_READABLE | ATTR_MUTABLE)

/*    Execute the ast
 */
bool xre_runtime(xre_ast_t *ast);

extern err_notif_t _error;

/*    sets the '_error' global struct with the appropriate info
 *    given a 'error_type_e'.
 */
void set_current_error(ast_stmt_t *self, error_type_e type);

#endif
