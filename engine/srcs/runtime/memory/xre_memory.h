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

#define __stack_top_set_attr(f) (__object_set_attr((object_t *)stack_top(), f))
#define __stack_top_has_attr(f) (__object_has_attr((object_t *)stack_top(), f))
#define __stack_top_unset_attr(f) \
	(__object_unset_attr((object_t *)stack_top(), f))

#define ATTR(name, shift) (1LL << (shift))

#define ATTR_STRING ATTR(STRING, 1) // is allocated on the heap
#define ATTR_REGISTER ATTR(REGISTER, 2) // fits in a register
#define ATTR_SEQUENCE ATTR(SEQUENCE, 3) // the beginning of a sequence
#define ATTR_SYMBOL ATTR(SYMBOL, 4) // is a reference to another object
#define ATTR_MUTABLE ATTR(MUTABLE, 5) // is mutable object
#define ATTR_READABLE ATTR(READABLE, 6) // is readable object
#define ATTR_REFERENCE ATTR(REFERENCE, 7) // is reference object
#define ATTR_CONSTANT ATTR(CONSTANT, 8) // is a constant object

#define __as_int64_t(obj_ptr) ((int64_t)(obj_ptr)->data.ptr)

/*    A representation of a value during runtime.
 */

#define __object_get_attr(object_ptr) ((object_ptr)->header.attrs)
#define __object_has_attr(object_ptr, __attr) \
	((object_ptr)->header.attrs & __attr)
#define __object_set_attr(object_ptr, __attr) \
	((object_ptr)->header.attrs |= __attr)
#define __object_unset_attr(object_ptr, __attr) \
	((object_ptr)->header.attrs &= ~(__attr))

#define __object_get_depth(object_ptr) ((object_ptr)->header.depth)
#define __object_set_depth(object_ptr, __depth) \
	((object_ptr)->header.depth = __depth)

#define __object_get_ref_count(object_ptr) ((object_ptr)->header.ref_count)
#define __object_set_ref_count(object_ptr, __ref_count) \
	((object_ptr)->header.ref_count = __ref_count)
#define __object_incr_ref_count(object_ptr) ((object_ptr)->header.ref_count++)
#define __object_decr_ref_count(object_ptr) ((object_ptr)->header.ref_count--)

#define __object_get_data_ptr(object_ptr) ((object_ptr)->data.ptr)
#define __object_set_data_ptr(object_ptr, __ptr) \
	((object_ptr)->data.ptr = (void *)__ptr)

#define __object_get_data_size(object_ptr) ((object_ptr)->data.size)
#define __object_set_data_size(object_ptr, __size) \
	((object_ptr)->data.size = __size)

// #define __object_set_invalid_address(object_ptr) \
// 	((object_ptr)->header.address = -1)
// #define __object_get_address(object_ptr) ((object_ptr)->header.address)

typedef struct {
	uint32_t attrs; // contains the object's attributes (declared above)
	uint32_t depth; // the level of { } the object is in
	uint64_t ref_count; // the number of other objects that reference this object
	// int64_t address;
} object_header_t;

typedef struct {
	object_header_t header;
	struct {
		union {
			void *ptr; // a pointer to the buffer
			int64_t value; // the value
		};
		size_t size; // the size of the data in bytes
	} data;

	void (*drop)(void *);
	void (*repr)(void *);
	bool (*is_true)(void *);
} object_t;

void object_attrs_repr(int32_t attrs);

object_t *object_register_create(int64_t data);
object_t *object_string_create(unsigned char *ptr, size_t size);
object_t *object_symbol_create(int64_t offset);
object_t *object_sequence_create(size_t depth, object_t *lval, object_t *rval);
object_t *object_undefined_create(void);

void object_drop(void *ptr);
bool is_true_object(const object_t *object);

bool stack_init(void);
void stack_fini(void);
bool stack_push(object_t *object);
void stack_pop(object_t *ptr);
void stack_pop_discard(void);
const object_t *stack_top(void);

bool symtab_init(void);
void symtab_fini(void);
int symtab_entry_create(const char *sym);

void heap_fini(void);
bool heap_init(void);

object_t *object_get(size_t address);
bool object_delete(object_t *object);
bool object_new(object_t *object);

#endif /* __XRE_MEMORY_H__ */
