#include "dynstr.h"
#include "xre_assert.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

dynstr_t *dynstr_new(size_t n)
{
	__return_val_if_fail__(SIZE_MAX - 1 > n, NULL);

	dynstr_t *str = NULL;
	size_t init_size = n + sizeof(char);

	str = malloc(sizeof(dynstr_t));
	if (unlikely(!str))
		return (NULL);

	str->_ptr = malloc(init_size);
	if (unlikely(!str->_ptr)) {
		free(str);
		return (NULL);
	}

	*str->_ptr = '\0';

	str->_cap = init_size;
	str->_size = sizeof(char);

	return (str);
}

dynstr_t *dynstr_from(const char *src, size_t n)
{
	__return_val_if_fail__(src, NULL);
	__return_val_if_fail__(SIZE_MAX - 1 > n, NULL);

	dynstr_t *str = NULL;
	size_t init_size = n + sizeof(char);

	str = malloc(sizeof(dynstr_t));
	if (unlikely(!str))
		return (NULL);

	str->_ptr = malloc(init_size);
	if (unlikely(!str->_ptr)) {
		free(str);
		return (NULL);
	}

	(void)dynstr_memcpy(str->_ptr, src, n);
	*(str->_ptr + n) = '\0';

	str->_cap = init_size;
	str->_size = n + 1;

	return (str);
}

bool dynstr_adjust(dynstr_t *self, size_t n)
{
	__return_val_if_fail__(SIZE_MAX > n, NULL);
	__return_val_if_fail__(self, NULL);

	size_t size;
	void *ptr;
	if (n + 1 < self->_cap)
		return (true);

	n < self->_cap << 1 ? (size = self->_cap << 1) : (size = n);

	ptr = realloc(self->_ptr, size);
	if (unlikely(!ptr))
		return (false);

	self->_cap = size;
	self->_ptr = ptr;
	return (true);
}

bool dynstr_append(dynstr_t *self, const char *src)
{
	__return_val_if_fail__(self, 0);
	__return_val_if_fail__(src, 0);

	size_t size = strlen(src);
	if (unlikely(!dynstr_adjust(self, self->_size + size)))
		return (false);

	(void)dynstr_memmove(self->_ptr + self->_size - 1, src, size + 1);

	self->_size += size;
	return (true);
}

dynstr_t *dynstr_assign(const char *src)
{
	__return_val_if_fail__(src, NULL);

	size_t str_size = strlen(src);

	dynstr_t *str = dynstr_new(str_size);
	if (unlikely(!str))
		return (NULL);

	(void)dynstr_memcpy(str->_ptr, src, str_size + sizeof(char));

	str->_size = str_size + sizeof(char);
	return (str);
}

bool dynstr_inject(dynstr_t *self, const char *src, size_t p)
{
	__return_val_if_fail__(self, false);
	__return_val_if_fail__(src, false);
	__return_val_if_fail__(p < self->_size, false);

	size_t str_size = strlen(src);

	if (unlikely(!dynstr_adjust(self, self->_size + str_size)))
		return (false);

	if (self->_size == sizeof(char))
		goto skip;

	(void)dynstr_memmove(self->_ptr + p + str_size, self->_ptr + p,
			     self->_size - p);

skip:
	(void)dynstr_memmove(self->_ptr + p, src, str_size);

	self->_size += str_size;
	return (true);
}

void dynstr_wipe(dynstr_t *self, size_t start, size_t end)
{
	__return_if_fail__(self);
	__return_if_fail__(start < end);
	__return_if_fail__(end - start <= self->_size);

	size_t __n = end - start;

	(void)dynstr_memmove(self->_ptr + start, self->_ptr + end,
			     self->_size - start - __n);

	self->_size -= __n;
}

void dynstr_kill(dynstr_t *self)
{
	__return_if_fail__(self);

	free(self->_ptr);
	free(self);

	self = NULL;
}

void dynstr_clear(dynstr_t *self)
{
	__return_if_fail__(self);

	self->_size = 0;
	*self->_ptr = '\0';
}

size_t dynstr_cap(dynstr_t *self)
{
	__return_val_if_fail__(self, -1);

	return (self->_cap);
}
