#include "vec.h"
#include "xre_compiler.h"
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* Aligns the size by the machine word.
 */
static inline size_t size_align(size_t n)
{
	return (n + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
}

static inline bool vec_init(struct vector **self, size_t size)
{
	*self = malloc(sizeof(**self));
	if (!*self) {
		return (false);
	}

	(void)memset(*self, 0x00, sizeof(struct vector));
	(*self)->_ptr = malloc(size);

	if (!(*self)->_ptr) {
		free(*self);
		return (false);
	}

	return (true);
}

struct vector *
vec_create(size_t elt_size, size_t n, void (*free)(void *))
{
	__trigger_bug_if(elt_size == 0);

	if (!n) {
		n = 64;
	}

	struct vector *v	= NULL;
	size_t	       init_cap = size_align(elt_size * n);

	if (vec_init(&v, init_cap)) {
		v->_elt_size = elt_size;
		v->_cap	     = init_cap;
		v->_free     = free;
	}

	return (v);
}

void *vec_extract(const struct vector *src, size_t start, size_t end)
{
	__trigger_bug_if(src == NULL);
	__trigger_bug_if((vec_size(src) - start) < (end - start));

	void *ptr = malloc((end - start) * src->_elt_size);

	if (ptr) {
		(void
		)memcpy(ptr,
			((char *)(src)->_ptr +
			 (src)->_elt_size * (start)),
			(end - start) * src->_elt_size);
	}

	return (ptr);
}

struct vector *
vec_pull(const struct vector *src, int64_t start, int64_t end)
{
	__trigger_bug_if(src == NULL);
	__trigger_bug_if(src->_nmemb <= (size_t)llabs(start));
	__trigger_bug_if(src->_nmemb <= (size_t)llabs(end));

	struct vector *v = NULL;

	if (start < 0) {
		start += src->_nmemb;
	}

	if (end < 0) {
		end += src->_nmemb;
	}

	start < end ? (end++) : (end--);

	size_t n_elems	  = llabs(start - end);
	size_t buffersize = n_elems * src->_elt_size;

	if (!vec_init(&v, buffersize)) {
		return (NULL);
	}

	v->_elt_size = src->_elt_size;
	v->_free     = src->_free;
	v->_cap	     = buffersize;
	v->_nmemb    = n_elems;

	if (start < end) {
		(void
		)memcpy(v->_ptr,
			((char *)(src)->_ptr +
			 (src)->_elt_size * (start)),
			buffersize);

	} else {
		void  *ptr  = v->_ptr;
		size_t step = src->_elt_size;

		while (start != end) {
			(void
			)memcpy(ptr,
				((char *)(src)->_ptr +
				 (src)->_elt_size * (start)),
				step);

			ptr = (char *)ptr + step;
			start--;
		}
	}

	return (v);
}

void vec_clear(struct vector *self)
{
	__trigger_bug_if(self == NULL);

	if (self->_free) {
		while (self->_nmemb--) {
			self->_free(
				((char *)(self)->_ptr +
				 (self)->_elt_size * (self->_nmemb))
			);
		}
	}

	self->_nmemb = 0;
}

void vec_kill(struct vector *self)
{
	__trigger_bug_if(self == NULL);

	vec_clear(self);
	free(self->_ptr);
	free(self);
}

bool vec_adjust(struct vector *self, size_t n)
{
	__trigger_bug_if(self == NULL);

	size_t new_size = 0;

	n += self->_nmemb;
	n *= self->_elt_size;

	if (n < self->_cap) {
		return (true);
	}

	size_t cap_2x = self->_cap * 2;

	if (cap_2x < 64) {
		cap_2x = 64;
	} else {
		if (cap_2x > 2147483647)
			return (false);
	}

	if (n > cap_2x) {
		new_size = size_align(n);
	} else {
		new_size = cap_2x;
	}

	void *ptr = realloc(self->_ptr, new_size);

	if (!ptr) {
		return (false);
	}

	self->_ptr = ptr;
	self->_cap = new_size;

	return (true);
}

bool vec_push(struct vector *self, const void *e)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(!e);

	if (!vec_adjust(self, 1)) {
		return (false);
	}

	(void)memmove(
		((char *)(self)->_ptr +
		 (self)->_elt_size * (self->_nmemb)),
		e,
		self->_elt_size
	);

	self->_nmemb++;

	return (true);
}

void vec_pop(struct vector *self, void *into)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(self->_nmemb == 0);

	self->_nmemb--;

	void *ptr =
		((char *)(self)->_ptr +
		 (self)->_elt_size * (self->_nmemb));

	if (into) {
		(void)memcpy(into, ptr, self->_elt_size);
	}

	if (self->_free) {
		self->_free(ptr);
	}
}

bool vec_pushf(struct vector *self, void *e)
{
	return (vec_insert(self, 0, e));
}

void vec_popf(struct vector *self, void *into)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(self->_nmemb == 0);

	if (into) {
		(void)memcpy(into, self->_ptr, self->_elt_size);
	}

	vec_evict(self, 0);
}

bool vec_insert(struct vector *self, size_t p, void *e)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if((p <= self->_nmemb) == false);

	if (!vec_adjust(self, 1)) {
		return (false);
	}

	if (!self->_nmemb || p == self->_nmemb) {
		goto skip;
	}

	(void)memmove(
		((char *)(self)->_ptr + (self)->_elt_size * (p + 1)),
		((char *)(self)->_ptr + (self)->_elt_size * (p)),
		self->_nmemb * self->_elt_size - p * self->_elt_size
	);

skip:
	(void
	)memcpy(((char *)(self)->_ptr + (self)->_elt_size * (p)),
		e,
		self->_elt_size);
	++self->_nmemb;

	return (true);
}

void vec_tipex(
	struct vector *self, size_t off, const void *src, size_t n
)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(src == NULL);

	(void)memmove((char *)self->_ptr + off, src, n);
}

struct vector *vec_dup(struct vector *self)
{
	__trigger_bug_if(self == NULL);

	struct vector *v = vec_create(
		self->_elt_size, self->_nmemb, self->_free
	);
	if (!v) {
		return (NULL);
	}

	if (!vec_concat(v, self)) {
		vec_kill(v);
		return (NULL);
	}

	return (v);
}

bool vec_inject(
	struct vector *self, size_t p, const void *src, size_t n
)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(src == NULL);
	__trigger_bug_if(p > self->_nmemb);

	if (!vec_adjust(self, n)) {
		return (false);
	}

	if (!n) {
		return (true);
	}

	if (!self->_nmemb || p == self->_nmemb) {
		goto skip_moving;
	}

	(void)memmove(
		((char *)(self)->_ptr + (self)->_elt_size * (p + n)),
		((char *)(self)->_ptr + (self)->_elt_size * (p)),
		self->_elt_size * (self->_nmemb - p)
	);

skip_moving:
	(void)memmove(
		((char *)(self)->_ptr + (self)->_elt_size * (p)),
		src,
		self->_elt_size * n
	);

	self->_nmemb += n;

	return (true);
}

bool vec_append(struct vector *self, const void *src, size_t n)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(src == NULL);

	if (!vec_adjust(self, n)) {
		return (false);
	}

	(void)__builtin_memmove(
		((char *)(self)->_ptr +
		 (self)->_elt_size * (self->_nmemb)),
		src,
		self->_elt_size * n
	);

	self->_nmemb += n;

	return (true);
}

bool vec_concat(struct vector *self, struct vector *other)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(other == NULL);
	__trigger_bug_if(self->_elt_size != other->_elt_size);

	if (!vec_adjust(self, vec_size(self) + vec_size(other))) {
		return (false);
	}

	(void)memmove(
		((char *)(self)->_ptr +
		 (self)->_elt_size * (self->_nmemb)),
		other->_ptr,
		vec_sizeof(other)
	);

	self->_nmemb += vec_size(other);

	return (true);
}

const void *vec_at(const struct vector *self, size_t p)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(p >= self->_nmemb);

	if (p >= self->_nmemb) {
		return (NULL);
	}

	return (((char *)(self)->_ptr + (self)->_elt_size * (p)));
}

const void *vec_unsafe_at(const struct vector *self, size_t p)
{
	return (((char *)(self)->_ptr + (self)->_elt_size * (p)));
}

void *vec_access(const struct vector *self, size_t p)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(self->_nmemb <= p);

	if (p >= self->_nmemb) {
		return (NULL);
	}

	return (((char *)(self)->_ptr + (self)->_elt_size * (p)));
}

void *vec_unsafe_access(const struct vector *self, size_t p)
{
	return (((char *)(self)->_ptr + (self)->_elt_size * (p)));
}

void vec_evict(struct vector *self, size_t p)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(p >= self->_nmemb);

	size_t n = (self->_nmemb - p) * self->_elt_size;

	if (self->_free) {
		self->_free((
			(char *)(self)->_ptr + (self)->_elt_size * (p)
		));
	}

	self->_nmemb--;

	if (p <= self->_nmemb) {
		(void)memmove(
			((char *)(self)->_ptr +
			 (self)->_elt_size * (p)),
			((char *)(self)->_ptr +
			 (self)->_elt_size * (p + 1)),
			n - self->_elt_size
		);
	}
}

void vec_wipe(struct vector *self, size_t start, size_t end)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(end - start > self->_nmemb);

	size_t n = end - start;

	if (self->_free) {
		size_t i = 0;

		while (i < n) {
			self->_free(
				((char *)(self)->_ptr +
				 (self)->_elt_size * (i))
			);
			i++;
		}
	}

	(void)memmove(
		((char *)(self)->_ptr + (self)->_elt_size * (start)),
		((char *)(self)->_ptr + (self)->_elt_size * (end)),
		(self->_nmemb - start - n) * self->_elt_size
	);

	self->_nmemb -= n;
}

void vec_swap_elems(struct vector *self, size_t a, size_t b)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(a >= self->_nmemb);
	__trigger_bug_if(b >= self->_nmemb);

	size_t n = self->_elt_size;

	char *p = ((char *)(self)->_ptr + (self)->_elt_size * (a));
	char *q = ((char *)(self)->_ptr + (self)->_elt_size * (b));

	for (; n--; ++p, ++q) {
		*p ^= *q;
		*q ^= *p;
		*p ^= *q;
	}
}
void *vec_head(const struct vector *self)
{
	return (vec_access(self, 0));
}

void *vec_tail(const struct vector *self)
{
	__trigger_bug_if(self == NULL);

	if (self->_nmemb) {
		return (vec_access(self, self->_nmemb - 1));
	}

	return (NULL);
}

size_t vec_size(const struct vector *self)
{
	__trigger_bug_if(self == NULL);

	return (self->_nmemb);
}

size_t vec_sizeof(const struct vector *self)
{
	__trigger_bug_if(self == NULL);

	return (self->_nmemb * self->_elt_size);
}

void *vec_data(const struct vector *self)
{
	return vec_head(self);
}

void *vec_uninitialized_data(const struct vector *self)
{
	__trigger_bug_if(self == NULL);
	__trigger_bug_if(self->_ptr == NULL);

	return (vec_unsafe_access(self, self->_nmemb));
}

size_t vec_uninitialized_size(const struct vector *self)
{
	__trigger_bug_if(self == NULL);

	size_t size_in_bytes = self->_cap - vec_sizeof(self);

	if (size_in_bytes) {
		size_in_bytes /= self->_elt_size;
	}

	return (size_in_bytes);
}

size_t vec_cap(const struct vector *self)
{
	__trigger_bug_if(self == NULL);

	return (self->_cap);
}

bool vec_append_from_capacity(struct vector *self, size_t n)
{
	__trigger_bug_if(self == NULL);

	if (n > vec_uninitialized_size(self)) {
		return (false);
	}

	self->_nmemb += n;

	return (true);
}

bool vec_slimcheck(struct vector *self)
{
	__trigger_bug_if(self == NULL);

	if (self->_cap) {
		size_t size = vec_sizeof(self);

		if (size < self->_cap / 2) {
			void *ptr = realloc(self->_ptr, size);

			if (!ptr) {
				return (false);
			}

			self->_ptr = ptr;
			self->_cap = size;
		}
	}

	return (true);
}
