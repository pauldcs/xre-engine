#ifndef __XRE_BUILIN_H__
#define __XRE_BUILIN_H__

#include "xre_runtime.h"
#include "xre_memory.h"
#include <stdbool.h>

typedef bool (*builtin_func_ptr)(ast_stmt_t *self);

// Define the struct type
typedef struct {
	const char *name;
	builtin_func_ptr func;
} builtin_func_t;

extern builtin_func_t builtin_func_lookup[4];

bool is_defined_builtin(const char *ptr, size_t size);
builtin_func_ptr get_builtin_ptr(const char *ptr, size_t size);
const char *get_builtin_name_ptr(const char *ptr, size_t size);

#define XRE_BUILTIN_FUNCTION(name) bool name(ast_stmt_t *self)

XRE_BUILTIN_FUNCTION(builtin_print);
XRE_BUILTIN_FUNCTION(builtin_typeof);
XRE_BUILTIN_FUNCTION(builtin_sizeof);
XRE_BUILTIN_FUNCTION(builtin_array_size);
XRE_BUILTIN_FUNCTION(builtin_array_at);
XRE_BUILTIN_FUNCTION(builtin_array_access);

#endif /* __XRE_BUILIN_H__ */
