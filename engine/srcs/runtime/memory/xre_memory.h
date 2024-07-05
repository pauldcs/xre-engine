#ifndef __XRE_MEMORY_H__
#define __XRE_MEMORY_H__

#include "array.h"
#include "xre_assert.h"
#include "xre_runtime.h"
#include "xre_object.h"
#include <stddef.h>
#include <stdbool.h>

#if !defined(STACK_SIZE)
#define STACK_SIZE 2048
#endif

extern array_t *g_stack;
extern size_t g_stack_ptr;
extern array_t *g_symtab;
extern array_t *g_symcache;

#define __stack_top_enable_flags(f) (((object_t *)stack_top())->flags |= (f))
#define STACK_TOP_CHECK_FLAGS(f) (((object_t *)stack_top())->flags & (f))
#define __stack_top_disable_flags(f) (((object_t *)stack_top())->flags &= ~(f))

bool stack_init(void);
void stack_fini(void);
bool stack_push(object_t *object);
const object_t *stack_top(void);
void stack_pop(object_t *ptr);
void stack_pop_discard(void);
void stack_debug(void);
void flags_to_string(int32_t flags);

int symtab_create_entry(const char *sym);
object_t *symtab_get_entry(int index);
bool symtab_init(void);
void symtab_fini(void);

#endif /* __XRE_MEMORY_H__ */
