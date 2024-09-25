#ifndef __XRE_RUNTIME_H__
#define __XRE_RUNTIME_H__

#include "xre_errors.h"
#include "xre_parse.h"
#include <stdbool.h>

/*    The parsed AST is tranformed into this 'struct statements'
 *    which is essencially the same except that the tree is
 *    in an array.
 */

typedef int statement_name_t;

struct statements {
	bool (*eval)(struct statements *);
	struct token *orig;
	vec_t	     *local;
	//vec_t *frame;
	union {
		int64_t value; // the value that the token represents
		char *string;  // the string that the token represents
		vec_t /* <statement_name_t> */ *children;
		struct {
			statement_name_t
				left; // index of the left child branch
			statement_name_t
				right; // index of the right child branch
		} br; // the operator that the token represents
	};
};

extern struct statements *__statements__;

// struct frame {
// 	statement_name_t statement;
// 	object_t *args[8];
// 	struct {
// 		size_t r;
// 		size_t w;
// 	} segments;
// };

#define __br_eval(obj_ptr) obj_ptr->eval(obj_ptr)

#define __push_r_as_ref(self_ptr, obj_ptr)                        \
	stack_push_enable_attrs(                                  \
		self_ptr, obj_ptr, ATTR_READABLE | ATTR_REFERENCE \
	)

#define __push_r(self_ptr, obj_ptr) \
	stack_push_enable_attrs(self_ptr, obj_ptr, ATTR_READABLE)

#define __push_w(self_ptr, obj_ptr) \
	stack_push_enable_attrs(self_ptr, obj_ptr, ATTR_MUTABLE)

#define __push_rw(self_ptr, obj_ptr)                            \
	stack_push_enable_attrs(                                \
		self_ptr, obj_ptr, ATTR_READABLE | ATTR_MUTABLE \
	)

/*    Execute the ast
 */
bool xre_runtime(struct ast *ast);

extern err_notif_t _error;

/*    sets the '_error' global struct with the appropriate info
 *    given a 'error_type_e'.
 */
void set_current_error(struct statements *self, error_type_e type);

#endif
