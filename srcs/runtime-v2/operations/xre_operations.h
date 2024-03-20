#ifndef __XRE_OPERATIONS_H__
#define __XRE_OPERATIONS_H__

#include "xre_runtime.h"
#include <stdbool.h>

/*    ARITHMETIC FUNCTIONS DEFINES
 */
# define XRE_ARITHMETIC_OPER_API(name) bool name(object_t* lval, object_t* rval)
typedef bool (*arithmetic_fptr_t)(object_t* lval, object_t* rval);

bool arithmetic_initiator(stmt_tree_t *tree);

XRE_ARITHMETIC_OPER_API(oper_add);
XRE_ARITHMETIC_OPER_API(oper_bw_and);
XRE_ARITHMETIC_OPER_API(oper_bw_or);
XRE_ARITHMETIC_OPER_API(oper_bw_xor);
XRE_ARITHMETIC_OPER_API(oper_div);
XRE_ARITHMETIC_OPER_API(oper_lshift);
XRE_ARITHMETIC_OPER_API(oper_rshift);
XRE_ARITHMETIC_OPER_API(oper_mod);
XRE_ARITHMETIC_OPER_API(oper_mul);
XRE_ARITHMETIC_OPER_API(oper_pow);
XRE_ARITHMETIC_OPER_API(oper_sub);

/*    ASSIGNMENT FUNCTIONS DEFINES
 */
# define XRE_ASSIGNMENT_OPER_API(name) bool name(object_t* dst, object_t* src)
typedef bool (*assignment_fptr_t)(object_t* dst, object_t* src);

bool assignment_initiator(stmt_tree_t *tree);

XRE_ASSIGNMENT_OPER_API(oper_add_assign);
XRE_ASSIGNMENT_OPER_API(oper_assign);
XRE_ASSIGNMENT_OPER_API(oper_div_assign);
XRE_ASSIGNMENT_OPER_API(oper_mod_assign);
XRE_ASSIGNMENT_OPER_API(oper_mul_assign);
XRE_ASSIGNMENT_OPER_API(oper_pow_assign);
XRE_ASSIGNMENT_OPER_API(oper_sub_assign);

/*    LOGICAL FUNCTIONS DEFINES
 */
# define XRE_LOGICAL_OPER_API(name) bool name(stmt_t condition, stmt_t consequence)
typedef bool (*logical_fptr_t)(stmt_t condition, stmt_t consequence);

bool logical_initiator(stmt_tree_t *tree);

XRE_LOGICAL_OPER_API(oper_and);
XRE_LOGICAL_OPER_API(oper_do);
XRE_LOGICAL_OPER_API(oper_else);
XRE_LOGICAL_OPER_API(oper_loop);
XRE_LOGICAL_OPER_API(oper_or);

/*    RELATIONAL FUNCTIONS DEFINES
 */
# define XRE_RELATIONAL_OPER_API(name) bool name(object_t* lval, object_t* rval)
typedef bool (*relational_fptr_t)(object_t* lval, object_t* rval);

bool relational_initiator(stmt_tree_t *tree);

XRE_RELATIONAL_OPER_API(oper_eq);
XRE_RELATIONAL_OPER_API(oper_ge);
XRE_RELATIONAL_OPER_API(oper_gt);
XRE_RELATIONAL_OPER_API(oper_le);
XRE_RELATIONAL_OPER_API(oper_lt);
XRE_RELATIONAL_OPER_API(oper_ne);
XRE_RELATIONAL_OPER_API(oper_not);

bool value_initiator(stmt_tree_t *tree);

#endif /* __XRE_OPERATIONS_H__ */
