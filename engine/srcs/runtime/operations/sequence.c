#include "xre_compiler.h"
#include "xre_log.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

static inline bool _oper_sequence(struct statement *self)
{
	__return_val_if_fail__(self, false);

	vec_t *sequence = vec_create(
		sizeof(object_t), vec_size(self->children), NULL
	);
	size_t	 i = 0;
	object_t object;
loop:
	if (!__br_eval((
		    &__statements__[*(int *)vec_at(self->children, i)]
	    ))) {
		return (false);
	}

	stack_pop(&object);
	vec_push(sequence, &object);
	i++;
	if (i < vec_size(self->children)) {
		goto loop;
	}

	object_sequence_init(sequence, &object);
	return (__push_r(self, &object));
}

XRE_API(oper_sequence)
{
	__trigger_bug_if(self == NULL);
	return (_oper_sequence(self));
}
