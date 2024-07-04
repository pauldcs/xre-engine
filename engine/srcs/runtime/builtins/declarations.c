#include "xre_builtin.h"

builtin_func_t builtin_func_lookup[] = { { "print", builtin_print },
					 { "typeof", builtin_typeof },
					 { "sizeof", builtin_sizeof } };
