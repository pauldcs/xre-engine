#ifndef __VEC_H__
#define __VEC_H__

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define vec(__type) struct vector
/*
 *   Fast vector "class"
 */
struct vector {
	void  *_ptr;   /* A pointer to the start of the buffer */
	size_t _nmemb; /* The number of elements in the buffer */
	size_t _cap; /* The size of the reserved memory block (in bytes) */
	size_t _elt_size; /* The size of one element (in bytes) */
	void (*_free)(void *); /* the element destructor function */
};

/* Creates a vec and adjusts its starting capacity to be at least
 * enough to hold 'n' elements.
 */
struct vector *
vec_create(size_t elt_size, size_t n, void (*free)(void *));

/* Reallocates the vec if more than half of the current capacity is unused.
 * If the reallocation fails the vec is untouched and the function
 * returns false.
 */
bool vec_slimcheck(struct vector *self);

/* Returns the number of elements contained in the vec.
 */
size_t vec_size(const struct vector *self);

/* Returns the number of bytes in use in the vec.
 */
size_t vec_sizeof(const struct vector *self);

/* Returns the number of bytes currently reserved by the array.
 */
size_t vec_cap(const struct vector *self);

/* Pointer to the underlying element storage. For non-empty containers,
 * the returned pointer compares equal to the address of the first element.
 */
void *vec_data(const struct vector *self);

/* Returns the number of elements that can fit in the buffer without
 * having to reallocate.
 */
size_t vec_uninitialized_size(const struct vector *self);

/* Pointer to the first uninitialized element in the buffer.
 */
void *vec_uninitialized_data(const struct vector *self);

/* Returns the data contained in 'self' in between start -> end into a newly
 * allocated buffer.
 */
void *vec_extract(const struct vector *src, size_t start, size_t end);

/* Extract and returns the data from 'self' in between start -> end (included)
 * into a newly allocated array. If a position is negative, it is iterpreted as
 * an offset from the end. If 'start' comes after 'end', the copy is made
 * backwards.
 *
 *   Example:
 *     let [a, b, c] be the array.
 * 	   vec_pull(v, 0, 2)   = [a, b, c].
 *     vec_pull(v, 0, -1)  = [a, b, c].
 *     vec_pull(v, 0, -2)  = [a, b].
 *     vec_pull(v, -1, 0)  = [c, b, a].
 *     vec_pull(v, -1, -1) = [c].
 *     vec_pull(v, -2, -1) = [b, c].
 */
struct vector *
vec_pull(const struct vector *src, int64_t start, int64_t end);

/* Frees the array, clearing the content beforhand.
 */
void vec_kill(struct vector *self);

/* Adds a new element at the end of the array, after its current
 * last element. The data pointed to by 'e' is copied (or moved) to the
 * new element.
 */
bool vec_push(struct vector *self, const void *e);

/* Removes the last element of the array, effectively reducing
 * the container size by one.
 */
void vec_pop(struct vector *self, void *into);

/* Adds a new element to the front of the array, before the
 * first element. The content of 'e' is copied (or moved) to the
 * new element.
 */
bool vec_pushf(struct vector *self, void *e);

/* Removes the first element from the array, reducing
 * the container size by one.
 */
void vec_popf(struct vector *self, void *into);

/* Creates a duplicate of 'self'
 */
struct vector *vec_dup(struct vector *self);

/* Copies 'n' bytes of data pointed to by 'src' directly into the array's
 * buffer, overwriting the data at the specified offset (in bytes).
 */
void vec_tipex(
	struct vector *self, size_t off, const void *src, size_t n
);

/* The array is extended by injecting a new element before the
 * element at the specified position, effectively increasing
 * the array's size by one.
 */
bool vec_insert(struct vector *self, size_t p, void *e);

/* Injects 'n' elements pointed to by 'src' into the array, at
 * potitions 'p'.
 */
bool vec_inject(
	struct vector *self, size_t p, const void *src, size_t n
);

/* Appends 'n' elements pointed to by 'src' into the array.
 */
bool vec_append(struct vector *self, const void *src, size_t n);

/* Appends 'n' elements pointed to by 'src' into the array.
 */
bool vec_concat(struct vector *self, struct vector *other);

/* Returns a pointer to the element at the specified position.
 */
void *vec_access(const struct vector *self, size_t p);
void *vec_unsafe_access(const struct vector *self, size_t p);

/* Identical to 'access', only this returns a const pointer.
 */
const void *vec_at(const struct vector *self, size_t p);
const void *vec_unsafe_at(const struct vector *self, size_t p);

/* Appends 'n' elements from capacity. The application must have initialized
 * the storage backing these elements otherwise the behavior is undefined.
 */
bool vec_append_from_capacity(struct vector *self, size_t n);

/* The element at position 'a' and the element at position 'b'
 * are swapped.
 */
void vec_swap_elems(struct vector *self, size_t a, size_t b);

/* Removes all elements from the array within start -> end
 * (which are ran through v->free), leaving the container with
 * a size of v->_nmemb - abs(start - end).
 */
void vec_wipe(struct vector *self, size_t start, size_t end);

/* Removes all the elements from the array and the capacity remains unchanged.
 */
void vec_clear(struct vector *self);

/* Removes the element at 'pos' from the array,
 * decreasing the size by one.
 */
void vec_evict(struct vector *self, size_t p);

/* Adjusts the array capacity to be at least enough to
 * contain the current + 'n' elements.
 */
bool vec_adjust(struct vector *self, size_t n);

/* Returns a pointer to the first element in the array.
 */
void *vec_head(const struct vector *self);

/* Returns a pointer to the last element in the array.
 */
void *vec_tail(const struct vector *self);

/* Swaps two arrays together array.
 */
void vec_swap(struct vector **self, struct vector **other);

#endif /* __VEC_H__*/
