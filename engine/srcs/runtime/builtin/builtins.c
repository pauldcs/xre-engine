#include "xre_builtin.h"
#include "xre_parse.h"

builtin_func_t builtin_func_lookup[] = {
	{ "print", BUILTIN_TYPE_UNIOP, builtin_print },
	{ "u8", BUILTIN_TYPE_UNIOP, builtin_u8buf },
	{ "str", BUILTIN_TYPE_UNIOP, builtin_str },
	{ "map", BUILTIN_TYPE_BINOP, builtin_map },
	{ "foreach", BUILTIN_TYPE_BINOP, builtin_foreach },
	{ "filter", BUILTIN_TYPE_BINOP, builtin_filter },
	{ "typeof", BUILTIN_TYPE_UNIOP, builtin_typeof }
};
