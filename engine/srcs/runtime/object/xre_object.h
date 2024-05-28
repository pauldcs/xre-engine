#ifndef __XRE_OBJECT_H__
# define __XRE_OBJECT_H__

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

typedef struct object_s object_t;

typedef void (*destructor_ptr)(void *);
typedef void (*reprfunc_ptr)(void *);
typedef bool (*testfunc_ptr)(void *);

#define FLAG(name, shift) (1LL << (shift))

#define FLAG_ALLOC    FLAG(ALLOC, 1) // is allocated on the heap
#define FLAG_REGISTER FLAG(REGISTER, 2) // fits in a register
#define FLAG_SEQUENCE FLAG(SEQUENCE, 3) // the beginning of a sequence
#define FLAG_SYMBOL   FLAG(SYMBOL, 4) // is a reference to another object

#define VALUE_OF(type, obj_ptr)  ((type)(obj_ptr)->data.ptr)

/*    A representation of a value during runtime.
 */
struct object_s {
  int32_t flags; // flags that indicate the behavior of the object
  struct {
    size_t size; // the size of the buffer
    void *ptr; // a pointer to the buffer
  } data;
  destructor_ptr dealloc; // frees the object
  reprfunc_ptr repr; // used by the print function to display the object
  testfunc_ptr test; // returns true/false based on the truthiness of the value
};

object_t *object_create_register(int64_t data);
object_t *object_create_slice(unsigned char *ptr, size_t size);
object_t *object_create_symbol(int64_t offset);

bool is_true_object(object_t *object);

#endif /* __XRE_OBJECT_H__ */
