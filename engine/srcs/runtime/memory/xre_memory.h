#ifndef __XRE_MEMORY_H__
#define __XRE_MEMORY_H__

#include "vec.h"
#include "dstr.h"
#include "xre_compiler.h"
#include "xre_runtime.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(STACK_SIZE)
#define STACK_SIZE 2048
#endif

//#define XRE_ENABLE_OBJECT_LOGGING 0

extern vec_t *g_stack;
extern size_t g_stack_ptr;
extern vec_t *g_symtab;

#define __stack_top_set_attr(f) \
	(__object_set_attr((object_t *)stack_top(), f))
#define __stack_top_has_attr(f) \
	(__object_has_attr((object_t *)stack_top(), f))
#define __stack_top_unset_attr(f) \
	(__object_unset_attr((object_t *)stack_top(), f))

#define ATTR(name, shift) (1LL << (shift))

#define ATTR_MUTABLE   ATTR(MUTABLE, 1)
#define ATTR_READABLE  ATTR(READABLE, 2)
#define ATTR_REFERENCE ATTR(REFERENCE, 3)
#define ATTR_CONSTANT  ATTR(CONSTANT, 4)
#define ATTR_NUMBER    ATTR(NUMBER, 5)
#define ATTR_SEQUENCE  ATTR(SEQUENCE, 7)
#define ATTR_BUFFER    ATTR(BUFFER, 8)
#define ATTR_STRING    ATTR(STRING, 9)
#define ATTR_UNDEFINED ATTR(UNDEFINED, 10)

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

#define __object_get_ref_count(object_ptr) \
	((object_ptr)->header.refcnt)
#define __object_set_ref_count(object_ptr, __ref_count) \
	((object_ptr)->header.refcnt = __ref_count)
#define __object_incr_ref_count(object_ptr) \
	((object_ptr)->header.refcnt++)
#define __object_decr_ref_count(object_ptr) \
	((object_ptr)->header.refcnt--)

#define __object_get_data_as_number(object_ptr) ((object_ptr)->number)
#define __object_set_data_as_number(object_ptr, __number) \
	((object_ptr)->number = (xre_number_t)__number)

#define __object_get_data_as_sequence(object_ptr) \
	((object_ptr)->sequence)
#define __object_set_data_as_sequence(object_ptr, __sequence) \
	((object_ptr)->sequence = (xre_sequence_t *)__sequence)
#define __object_set_data_as_buffer(object_ptr, __buffer) \
	((object_ptr)->buffer = (xre_buffer_t *)__buffer)

#define __object_get_data_as_string(object_ptr) ((object_ptr)->string)
#define __object_set_data_as_string(object_ptr, __string) \
	((object_ptr)->string = (xre_string_t *)__string)

#define __object_get_data_as_any(object_ptr) ((object_ptr)->any)
#define __object_set_data_as_any(object_ptr, __any) \
	((object_ptr)->any = (void *)__any)

// #define __object_set_invalid_address(object_ptr) \
// 	((object_ptr)->header.address = -1)
// #define __object_get_address(object_ptr) ((object_ptr)->header.address)

typedef dstr_t	xre_string_t;
typedef vec_t	xre_sequence_t;
typedef vec_t	xre_buffer_t;
typedef int64_t xre_number_t;
// typedef struct {
// 	uint8_t *ptr;
// 	size_t size;
// } xre_bytes_t;

typedef struct {
	uint64_t refcnt; // unused
	uint32_t attrs;
	uint32_t depth;
} object_header_t;

/*    A representation of a value during runtime.
 */
typedef struct {
	object_header_t header;
	union {
		void	       *any;
		xre_string_t   *string;
		xre_buffer_t   *buffer;
		xre_sequence_t *sequence;
		xre_number_t	number;
		//xre_bytes_t  bytes;
	};

	void (*drop)(void *);
	void (*repr)(void *);
	bool (*is_true)(void *);
} object_t;

void object_attrs_repr(int32_t attrs);

void object_number_init(int64_t data, object_t *buf);
bool object_string_init(unsigned char *string, object_t *buf);
bool object_buffer_init(object_t *source, object_t *buf);
bool object_sequence_init(vec_t *sequence, object_t *buf);
//object_t *object_bytes_create(void *ptr, size_t size);

void object_undefined_init(object_t *buf);

void object_drop(void *ptr);
bool is_true_object(const object_t *object);

bool		stack_init(void);
void		stack_fini(void);
bool		stack_push(object_t *object);
void		stack_pop(object_t *ptr);
void		stack_pop_discard(void);
const object_t *stack_top(void);

bool symtab_init(void);
void symtab_fini(void);

void heap_fini(void);
bool heap_init(void);

#endif /* __XRE_MEMORY_H__ */
