#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

/*   The 'do' operator is similar to the '&&' (and) operator,
 *   except that it always returns it's condition instead of
 *   it's consequence. 
 * 
 *   eg.
 *       a && b || c      'c' gets returned when 'a' or 'b' is false.
 *       a do b else c    'c' gets returned only when 'a' is false.
 * 
 *   in the second one, 'b' has no impact on whether 'c' gets returned
 *   or not.
 *   
 *   This behavior is required in order to chain multiple logical
 *   conditions together.
 * 
 *            a do b     //      if (a) { b }
 *       else c do d     // else if (c) { d }
 *       else e else f   // else {
 *                       //    if (e) { e } else { f }
 *                       // }
 *    
 */
static inline bool _oper_do(struct statements *self)
{
	if (!__br_eval(__left_branch)) {
		return (false);
	}

	if (!is_true_object(stack_top())) {
		return (true);
	}

	//	stack_pop(&object);

	if (!__br_eval(__right_branch)) {
		return (false);
	}

	stack_pop_discard();

	return (true);
	//	return (__push_r(self, &object));
}

XRE_API(oper_do)
{
	__trigger_bug_if(self == NULL);

	bool ret = _oper_do(self);
	return (ret);
}
