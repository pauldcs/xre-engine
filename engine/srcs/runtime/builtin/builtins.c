#include "xre_builtin.h"
#include "xre_parse.h"

builtin_func_t builtin_func_lookup[] = {
	{ "print", BUILTIN_TYPE_UNIOP, builtin_print },
	{ "str_size", BUILTIN_TYPE_UNIOP, builtin_str_size },
	{ "array_size", BUILTIN_TYPE_UNIOP, builtin_array_size },
	{ "yes", BUILTIN_TYPE_VALUE, builtin_yes },
	{ "typeof", BUILTIN_TYPE_UNIOP, builtin_typeof }
};
