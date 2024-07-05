#include "xre_builtin.h"

builtin_func_t builtin_func_lookup[] = { { "print", builtin_print },
					 { "array_size", builtin_array_size },
					 { "typeof", builtin_typeof },
					 { "sizeof", builtin_sizeof } };
