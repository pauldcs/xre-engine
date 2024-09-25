#include "xre_builtin.h"
#include <string.h>

struct builtin builtin_lookup[] = {
	{ "dbg", BUILTIN_UNIOP, NULL },
	{ "buf", BUILTIN_UNIOP, NULL },
	{ "str", BUILTIN_UNIOP, NULL },
	{ "map", BUILTIN_BINOP, NULL },
	{ "out", BUILTIN_UNIOP, NULL },
	{ "each", BUILTIN_BINOP, NULL },
	{ "filter", BUILTIN_BINOP, NULL },
	{ "typeof", BUILTIN_UNIOP, NULL }
};

static bool builtin_find(const char *ptr, size_t size, size_t *offset)
{
	size_t lookup_size =
		sizeof(builtin_lookup) / sizeof(struct builtin);
	size_t i = 0;

	while (i < lookup_size) {
		const char *name = builtin_lookup[i].iden;
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
	return (builtin_find(ptr, size, &offset));
}

const char *builtin_get_name(const char *ptr, size_t size)
{
	size_t offset;

	if (!builtin_find(ptr, size, &offset)) {
		return (NULL);
	}

	return (builtin_lookup[offset].iden);
}

enum builtin_type builtin_get_type(const char *ptr, size_t size)
{
	size_t offset;

	if (!builtin_find(ptr, size, &offset)) {
		return (0);
	}
	return (builtin_lookup[offset].type);
}
