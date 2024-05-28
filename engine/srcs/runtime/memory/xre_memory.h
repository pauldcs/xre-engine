#ifndef __XRE_MEMORY_H__
# define __XRE_MEMORY_H__

# include "array.h"
# include "xre_assert.h"
# include "xre_runtime.h"
# include "xre_object.h"
# include <stddef.h>
# include <stdbool.h>

# if !defined(STACK_SIZE)
#  define STACK_SIZE 2048
# endif

extern array_t *g_stack;
extern size_t   g_stack_ptr;
extern array_t *g_symtab;
extern array_t *g_symcache;

bool stack_init(void);
void stack_fini(void);
bool stack_push(object_t *object);
const object_t *stack_top(void);
void stack_pop(object_t *ptr);
void stack_pop_expand(object_t *ptr);
void stack_debug(void);

int symtab_create_entry(const char *sym);
object_t *symtab_get_entry(int index);
bool symtab_init(void);
void symtab_fini(void);

#endif /* __XRE_MEMORY_H__ */
