#include "xre_builtin.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static bool
get_builtin_offset(const char *ptr, size_t size, size_t *offset)
{
	size_t lookup_size =
		sizeof(builtin_func_lookup) / sizeof(builtin_func_t);
	size_t i = 0;

	while (i < lookup_size) {
		const char *name = builtin_func_lookup[i].name;
		if (strlen(name) == size &&
		    !memcmp(name, ptr, size)) {
			*offset = i;
			return (true);
		}
		i++;
	}

	return (false);
}

bool is_defined_builtin(const char *ptr, size_t size)
{
	size_t offset;
	return (get_builtin_offset(ptr, size, &offset));
}

builtin_func_ptr get_builtin_ptr(const char *ptr, size_t size)
{
	size_t offset;

	if (!get_builtin_offset(ptr, size, &offset)) {
		return (NULL);
	}

	return (builtin_func_lookup[offset].func);
}

const char *get_builtin_name_ptr(const char *ptr, size_t size)
{
	size_t offset;

	if (!get_builtin_offset(ptr, size, &offset)) {
		return (NULL);
	}

	return (builtin_func_lookup[offset].name);
}

xre_expr_type_t get_builtin_type(const char *ptr, size_t size)
{
	size_t offset;

	if (!get_builtin_offset(ptr, size, &offset)) {
		return (0);
	}
	const builtin_type_e type = builtin_func_lookup[offset].type;
	switch (type) {
	case BUILTIN_TYPE_BINOP:
		return EXPR_OP_TYPE_BINOP;

	case BUILTIN_TYPE_UNIOP:
		return EXPR_OP_TYPE_UNIOP;

	case BUILTIN_TYPE_VALUE:
		/* fallthrough */

	default:
		return EXPR_TYPE_VALUE;
	}
	return (0);
}
