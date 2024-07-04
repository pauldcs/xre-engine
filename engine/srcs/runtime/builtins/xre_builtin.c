#include "xre_builtin.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static bool get_builtin_offset(const char *ptr, size_t size, size_t *offset)
{
	size_t lookup_size =
		sizeof(builtin_func_lookup) / sizeof(builtin_func_t);
	size_t i = 0;

	while (i < lookup_size) {
		const char *name = builtin_func_lookup[i].name;
		if (strlen(name) == size && !memcmp(name, ptr, size)) {
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
		return (false);
	}

	return (builtin_func_lookup[offset].func);
}
