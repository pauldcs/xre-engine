#include "xre_builtin.h"
#include "xre_parse.h"

builtin_func_t builtin_func_lookup[] = {
	{ "u8", BUILTIN_TYPE_UNIOP, builtin_u8buf },
	{ "str", BUILTIN_TYPE_UNIOP, builtin_str },
	{ "map", BUILTIN_TYPE_BINOP, builtin_map },
	{ "print", BUILTIN_TYPE_UNIOP, builtin_print },
	{ "foreach", BUILTIN_TYPE_BINOP, builtin_foreach },
	{ "filter", BUILTIN_TYPE_BINOP, builtin_filter },
	{ "typeof", BUILTIN_TYPE_UNIOP, builtin_typeof }
};
