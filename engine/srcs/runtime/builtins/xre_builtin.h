#ifndef __XRE_BUILIN_H__
#define __XRE_BUILIN_H__

#include "xre_runtime.h"
#include "xre_memory.h"
#include "xre_object.h"
#include <stdbool.h>

typedef bool (*builtin_func_ptr)(ast_stmt_t *self);

// Define the struct type
typedef struct {
    const char *name;
    builtin_func_ptr func;
} builtin_func_t;

extern builtin_func_t builtin_func_lookup[];

bool is_defined_builtin(const char *ptr, size_t size);
builtin_func_ptr get_builtin_ptr(const char *ptr, size_t size);

# define XRE_BUILTIN_FUNCTION(name) bool name(ast_stmt_t *self)

XRE_BUILTIN_FUNCTION(builtin_print);
XRE_BUILTIN_FUNCTION(builtin_debug);

#endif /* __XRE_BUILIN_H__ */
