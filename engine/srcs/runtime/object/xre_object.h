#ifndef __XRE_OBJECT_H__
#define __XRE_OBJECT_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void (*destructor_ptr)(void *);
typedef void (*reprfunc_ptr)(void *);
typedef bool (*testfunc_ptr)(void *);

#define FLAG(name, shift) (1LL << (shift))

#define FLAG_ALLOC FLAG(ALLOC, 1) // is allocated on the heap
#define FLAG_REGISTER FLAG(REGISTER, 2) // fits in a register
#define FLAG_SEQUENCE FLAG(SEQUENCE, 3) // the beginning of a sequence
#define FLAG_SYMBOL FLAG(SYMBOL, 4) // is a reference to another object
#define FLAG_MUTABLE FLAG(MUTABLE, 5) // is mutable object
#define FLAG_READABLE FLAG(READABLE, 6) // is readable object

#define __as_int64_t(obj_ptr) ((int64_t)(obj_ptr)->data.ptr)

#define CREATE_DEFAULT_REGISTER
/*    A representation of a value during runtime.
 */
typedef struct {
	int32_t flags; // flags that indicate the behavior of the object
	size_t depth;
	struct {
		size_t size; // the size of the buffer
		void *ptr; // a pointer to the buffer
	} data;
	destructor_ptr dealloc; // frees the object
	reprfunc_ptr repr; // used by the print function to display the object
	testfunc_ptr
		test; // returns true/false based on the truthiness of the value
} object_t;

object_t *object_create_register(int64_t data);
object_t *object_create_slice(unsigned char *ptr, size_t size);
object_t *object_create_symbol(int64_t offset);
object_t *object_create_sequence(object_t *lval, object_t *rval);
object_t *object_create_wrapped_sequence(object_t *lval, object_t *rval);
object_t *object_create_undefined(void);

bool is_true_object(const object_t *object);

#endif /* __XRE_OBJECT_H__ */
