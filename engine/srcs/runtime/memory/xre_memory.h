#ifndef __XRE_MEMORY_H__
#define __XRE_MEMORY_H__

#include "array.h"
#include "xre_assert.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(STACK_SIZE)
#define STACK_SIZE 2048
#endif

//#define XRE_ENABLE_OBJECT_LOGGING 0

extern array_t *g_stack;
extern size_t g_stack_ptr;
extern array_t *g_symtab;
extern array_t *g_symcache;

#define __stack_top_enable_attr(f) (((object_t *)stack_top())->attrs |= (f))
#define __stack_top_check_attr(f) (((object_t *)stack_top())->attrs & (f))
#define __stack_top_disable_attr(f) (((object_t *)stack_top())->attrs &= ~(f))

#define ATTR(name, shift) (1LL << (shift))

#define ATTR_STRING ATTR(STRING, 1) // is allocated on the heap
#define ATTR_REGISTER ATTR(REGISTER, 2) // fits in a register
#define ATTR_SEQUENCE ATTR(SEQUENCE, 3) // the beginning of a sequence
#define ATTR_SYMBOL ATTR(SYMBOL, 4) // is a reference to another object
#define ATTR_MUTABLE ATTR(MUTABLE, 5) // is mutable object
#define ATTR_READABLE ATTR(READABLE, 6) // is readable object
#define ATTR_REFERENCE ATTR(REFERENCE, 7) // is reference object

#define __as_int64_t(obj_ptr) ((int64_t)(obj_ptr)->data.ptr)

/*    A representation of a value during runtime.
 */

typedef struct {
	int32_t attrs; // attributes that indicate the behavior of the object
	size_t depth;
	struct {
		size_t size; // the size of the buffer
		// union {
		void *ptr; // a pointer to the buffer
		// int64_t value; // the value
		// };
	} data;

	void (*drop)(void *);
	void (*repr)(void *);
	bool (*is_true)(void *);
} object_t;

void object_attrs_repr(int32_t attrs);

object_t *object_create_register(int64_t data);
object_t *object_create_string(unsigned char *ptr, size_t size);
object_t *object_create_symbol(int64_t offset);
object_t *object_create_sequence(object_t *lval, object_t *rval);
object_t *object_create_undefined(void);

bool is_true_object(const object_t *object);

void object_drop(void *ptr);

bool stack_init(void);
void stack_fini(void);
bool stack_push(object_t *object);

const object_t *stack_top(void);
void stack_pop(object_t *ptr);
void stack_pop_discard(void);

int symtab_create_entry(const char *sym);
object_t *symtab_get_entry(int index);
bool symtab_init(void);
void symtab_fini(void);

#endif /* __XRE_MEMORY_H__ */
