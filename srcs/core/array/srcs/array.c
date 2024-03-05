#include "array.h"
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

array_allocator_t __array_allocator__ = {
    ._memory_alloc = malloc, ._memory_realloc = realloc, ._memory_free = free};

/* Aligns the size by the machine word.
 */
static inline SIZE_TYPE(size_align)(SIZE_TYPE(n)) {
  return (n + sizeof(PTR_TYPE()) - 1) & ~(sizeof(PTR_TYPE()) - 1);
}

static inline BOOL_TYPE(array_init)(ARRAY_TYPE(*self), size_t size) {
  *self = __array_allocator__._memory_alloc(sizeof(**self));

  if (unlikely(!*self)) {
    return (false);
  }

  (void)builtin_memset(*self, 0x00, sizeof(array_t));

  _data((*self)) = __array_allocator__._memory_alloc(size);

  if (unlikely(!_data((*self)))) {
    __array_allocator__._memory_free(*self);
    return (false);
  }

  return (true);
}

ARRAY_TYPE(array_create)
(SIZE_TYPE(elt_size), SIZE_TYPE(n), void (*free)(void *)) {
  HR_COMPLAIN_IF(elt_size == 0);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL(elt_size, n) == false);

  if (!n) {
    n = ARRAY_INITIAL_SIZE;
  }

  ARRAY_TYPE(array) = NULL;
  SIZE_TYPE(init_cap) = size_align(elt_size * n);

  if (likely(array_init(&array, init_cap))) {
    _typesize(array) = elt_size;
    _capacity(array) = init_cap;
    _freefunc(array) = free;
    _is_owner(array) = true;
  }

  return (array);
}

ARRAY_TYPE(array_seize_buffer)
(PTR_TYPE(*buffer), SIZE_TYPE(bufsize), SIZE_TYPE(elt_size), SIZE_TYPE(n),
 void (*_free)(void *)) {
  HR_COMPLAIN_IF(elt_size == 0);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL(elt_size, n) == false);

  ARRAY_TYPE(self) = NULL;

  self = __array_allocator__._memory_alloc(sizeof(*self));

  if (likely(self)) {
    (void)builtin_memset(self, 0x00, sizeof(array_t));
    _capacity(self) = bufsize;
    _size(self) = n;
    _typesize(self) = elt_size;
    _data(self) = buffer;
    _freefunc(self) = _free;
    _is_owner(self) = true;
    _settled(self) = false;
  }

  return (self);
}

ARRAY_TYPE(array_borrow_buffer)
(PTR_TYPE(*buffer), SIZE_TYPE(bufsize), SIZE_TYPE(elt_size), SIZE_TYPE(n),
 void (*_free)(void *)) {
  HR_COMPLAIN_IF(elt_size == 0);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL(elt_size, n) == false);

  ARRAY_TYPE(self) = NULL;

  self = __array_allocator__._memory_alloc(sizeof(*self));

  if (likely(self)) {
    (void)builtin_memset(self, 0x00, sizeof(array_t));
    _capacity(self) = bufsize;
    _size(self) = n;
    _typesize(self) = elt_size;
    _data(self) = buffer;
    _freefunc(self) = _free;
    _is_owner(self) = false;
    _settled(self) = true;
  }

  return (self);
}

ARRAY_TYPE(array_filter)
(RDONLY_ARRAY_TYPE(self), bool (*callback)(RDONLY_PTR_TYPE(elem))) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(callback == NULL);

  ARRAY_TYPE(array) =
      array_create(_typesize(self), ARRAY_INITIAL_SIZE, _freefunc(self));

  if (unlikely(!array)) {
    return (NULL);
  }

  SIZE_TYPE(size) = array_size(self);
  SIZE_TYPE(i) = 0;

  while (i < size) {
    RDONLY_ARRAY_TYPE(elem) = array_at(self, i);
    if (!elem)
      goto error;

    if (callback(elem) && !array_push(array, elem))
      goto error;

    i++;
  }

  return (array);

error:
  array_kill(array);
  return (NULL);
}

PTR_TYPE(array_extract)
(RDONLY_ARRAY_TYPE(src), SIZE_TYPE(start), SIZE_TYPE(end)) {
  HR_COMPLAIN_IF(src == NULL);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_SUB(end, start) == false);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL((end - start), _typesize(src)) == false);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_SUB(_size(src), start) == false);
  HR_COMPLAIN_IF((_size(src) - start) < (end - start));

  PTR_TYPE(ptr) =
      __array_allocator__._memory_alloc((end - start) * _typesize(src));

  if (likely(ptr)) {
    (void)builtin_memcpy(ptr, _relative_data(src, start),
                         (end - start) * _typesize(src));
  }

  return (ptr);
}

ARRAY_TYPE(array_pull)
(RDONLY_ARRAY_TYPE(src), SSIZE_TYPE(start), SSIZE_TYPE(end)) {
  HR_COMPLAIN_IF(src == NULL);
  HR_COMPLAIN_IF(_size(src) <= ABS(start));
  HR_COMPLAIN_IF(_size(src) <= ABS(end));

  ARRAY_TYPE(arr) = NULL;

  if (start < 0) {
    start += _size(src);
  }

  if (end < 0) {
    end += _size(src);
  }

  start < end ? (end++) : (end--);

  SIZE_TYPE(n_elems) = labs(start - end);
  SIZE_TYPE(buffersize) = n_elems * _typesize(src);

  if (unlikely(!array_init(&arr, buffersize))) {
    return (NULL);
  }

  _typesize(arr) = _typesize(src);
  _freefunc(arr) = _freefunc(src);
  _capacity(arr) = buffersize;
  _size(arr) = n_elems;
  _settled(arr) = true;
  _is_owner(arr) = true;

  if (start < end) {
    (void)builtin_memcpy(_data(arr), _relative_data(src, start), buffersize);

  } else {
    PTR_TYPE(ptr) = _data(arr);
    SIZE_TYPE(step) = _typesize(src);

    while (start != end) {
      (void)builtin_memcpy(ptr, _relative_data(src, start), step);
      ptr = (char *)ptr + step;
      start--;
    }
  }

  return (arr);
}

NONE_TYPE(array_clear)(ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  if (_freefunc(self)) {
    while (_size(self)--) {
      _freefunc(self)(_relative_data(self, _size(self)));
    }
  }

  _size(self) = 0;
}

NONE_TYPE(array_kill)(ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  array_clear(self);

  if (_is_owner(self)) {
    __array_allocator__._memory_free(_data(self));
  }

  __array_allocator__._memory_free(self);
}

BOOL_TYPE(array_adjust)(ARRAY_TYPE(self), SIZE_TYPE(n)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL(_capacity(self), 2) == false);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_ADD(_size(self), n) == false);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_MUL(_size(self) + n, _typesize(self)) == false);

  SIZE_TYPE(new_size) = 0;

  n += _size(self);
  n *= _typesize(self);

  if (likely(n < _capacity(self))) {
    return (true);
  }

  if (unlikely(_settled(self))) {
    return (false);
  }

  SIZE_TYPE(cap_2x) = _capacity(self) * 2;

  if (cap_2x < ARRAY_INITIAL_SIZE) {
    cap_2x = ARRAY_INITIAL_SIZE;
  } else {
    if (unlikely(cap_2x > SIZE_TYPE_MAX))
      return (false);
  }

  if (n > cap_2x) {
    new_size = size_align(n);
  } else {
    new_size = cap_2x;
  }

  PTR_TYPE(ptr) = __array_allocator__._memory_realloc(_data(self), new_size);

  if (unlikely(!ptr)) {
    return (false);
  }

  _data(self) = ptr;
  _capacity(self) = new_size;

  return (true);
}

BOOL_TYPE(array_push)(ARRAY_TYPE(self), RDONLY_PTR_TYPE(e)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(!e);

  if (unlikely(!array_adjust(self, 1))) {
    return (false);
  }

  (void)builtin_memmove(_relative_data(self, _size(self)), e, _typesize(self));

  _size(self)++;

  return (true);
}

NONE_TYPE(array_pop)(ARRAY_TYPE(self), PTR_TYPE(into)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(_size(self) == 0);

  _size(self)--;

  PTR_TYPE(ptr) = _relative_data(self, _size(self));

  if (into) {
    (void)builtin_memcpy(into, ptr, _typesize(self));
  }

  if (_freefunc(self)) {
    _freefunc(self)(ptr);
  }
}

BOOL_TYPE(array_pushf)(ARRAY_TYPE(self), PTR_TYPE(e)) {
  return (array_insert(self, 0, e));
}

NONE_TYPE(array_popf)(ARRAY_TYPE(self), PTR_TYPE(into)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(_size(self) == 0);

  if (into) {
    (void)builtin_memcpy(into, _data(self), _typesize(self));
  }

  array_evict(self, 0);
}

BOOL_TYPE(array_insert)(ARRAY_TYPE(self), SIZE_TYPE(p), PTR_TYPE(e)) {
  HR_COMPLAIN_IF(self == false);
  HR_COMPLAIN_IF((p <= _size(self)) == false);

  if (unlikely(!array_adjust(self, 1))) {
    return (false);
  }

  if (!_size(self) || p == _size(self)) {
    goto skip;
  }

  (void)builtin_memmove(_relative_data(self, p + 1), _relative_data(self, p),
                        _size(self) * _typesize(self) - p * _typesize(self));

skip:
  (void)builtin_memcpy(_relative_data(self, p), e, _typesize(self));
  ++_size(self);

  return (true);
}

NONE_TYPE(array_tipex)
(ARRAY_TYPE(self), SIZE_TYPE(off), RDONLY_PTR_TYPE(src), SIZE_TYPE(n)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(src == NULL);

  (void)builtin_memmove((char *)_data(self) + off, src, n);
}

BOOL_TYPE(array_inject)
(ARRAY_TYPE(self), SIZE_TYPE(p), RDONLY_PTR_TYPE(src), SIZE_TYPE(n)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(src == NULL);
  HR_COMPLAIN_IF(p > _size(self));
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_ADD(p, n) == false);

  if (unlikely(!array_adjust(self, n))) {
    return (false);
  }

  if (unlikely(!n)) {
    return (true);
  }

  if (!_size(self) || p == _size(self)) {
    goto skip_moving;
  }

  (void)builtin_memmove(_relative_data(self, p + n), _relative_data(self, p),
                        _typesize(self) * (_size(self) - p));

skip_moving:
  (void)builtin_memmove(_relative_data(self, p), src, _typesize(self) * n);
  _size(self) += n;

  return (true);
}

BOOL_TYPE(array_append)(ARRAY_TYPE(self), RDONLY_PTR_TYPE(src), SIZE_TYPE(n)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(src == NULL);

  if (unlikely(!array_adjust(self, n))) {
    return (false);
  }

  (void)builtin_memmove(_relative_data(self, _size(self)), src,
                        _typesize(self) * n);

  _size(self) += n;

  return (true);
}

__attr_pure RDONLY_PTR_TYPE(array_at)(RDONLY_ARRAY_TYPE(self), SIZE_TYPE(p)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(p >= _size(self));

  if (unlikely(p >= _size(self))) {
    return (NULL);
  }

  return (_relative_data(self, p));
}

__attr_pure RDONLY_PTR_TYPE(array_unsafe_at)(RDONLY_ARRAY_TYPE(self),
                                             SIZE_TYPE(p)) {
  return (_relative_data(self, p));
}

__attr_pure PTR_TYPE(array_access)(RDONLY_ARRAY_TYPE(self), SIZE_TYPE(p)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(_size(self) <= p);

  if (unlikely(p >= _size(self))) {
    return (NULL);
  }

  return (_relative_data(self, p));
}

__attr_pure PTR_TYPE(array_unsafe_access)(RDONLY_ARRAY_TYPE(self),
                                          SIZE_TYPE(p)) {
  return (_relative_data(self, p));
}

NONE_TYPE(array_evict)(ARRAY_TYPE(self), SIZE_TYPE(p)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(p >= _size(self));

  SIZE_TYPE(n) = (_size(self) - p) * _typesize(self);

  if (_freefunc(self)) {
    _freefunc(self)(_relative_data(self, p));
  }

  _size(self)--;

  if (p <= _size(self)) {
    (void)builtin_memmove(_relative_data(self, p), _relative_data(self, p + 1),
                          n - _typesize(self));
  }
}

NONE_TYPE(array_wipe)(ARRAY_TYPE(self), SIZE_TYPE(start), SIZE_TYPE(end)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_SUB(end, start) == false);
  HR_COMPLAIN_IF(end - start > _size(self));

  SIZE_TYPE(n) = end - start;

  if (_freefunc(self)) {
    size_t i = 0;

    while (i < n) {
      _freefunc(self)(_relative_data(self, i));
      i++;
    }
  }

  (void)builtin_memmove(_relative_data(self, start), _relative_data(self, end),
                        (_size(self) - start - n) * _typesize(self));

  _size(self) -= n;
}

NONE_TYPE(array_swap_elems)(ARRAY_TYPE(self), SIZE_TYPE(a), SIZE_TYPE(b)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(a >= _size(self));
  HR_COMPLAIN_IF(b >= _size(self));

  SIZE_TYPE(n) = _typesize(self);

  char *p = _relative_data(self, a);
  char *q = _relative_data(self, b);

  for (; n--; ++p, ++q) {
    *p ^= *q;
    *q ^= *p;
    *p ^= *q;
  }
}

__attr_pure PTR_TYPE(array_head)(RDONLY_ARRAY_TYPE(self)) {
  return (array_access(self, 0));
}

__attr_pure PTR_TYPE(array_tail)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  if (likely(_size(self))) {
    return (array_access(self, _size(self) - 1));
  }

  return (NULL);
}

__attr_pure SIZE_TYPE(array_size)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  return (_size(self));
}

__attr_pure SIZE_TYPE(array_sizeof)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  return (_size(self) * _typesize(self));
}

__attr_pure PTR_TYPE(array_data)(RDONLY_ARRAY_TYPE(self)) {
  return array_head(self);
}

__attr_pure PTR_TYPE(array_uninitialized_data)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(_data(self) == NULL);

  return (array_unsafe_access(self, _size(self)));
}

__attr_pure SIZE_TYPE(array_uninitialized_size)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(SIZE_T_SAFE_TO_SUB(_capacity(self), array_sizeof(self)) ==
                 false);

  SIZE_TYPE(size_in_bytes) = _capacity(self) - array_sizeof(self);

  if (size_in_bytes) {
    size_in_bytes /= _typesize(self);
  }

  return (size_in_bytes);
}

__attr_pure SIZE_TYPE(array_cap)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == false);

  return (_capacity(self));
}

BOOL_TYPE(array_append_from_capacity)(ARRAY_TYPE(self), SIZE_TYPE(n)) {
  HR_COMPLAIN_IF(self == NULL);

  if (n > array_uninitialized_size(self)) {
    return (false);
  }

  _size(self) += n;

  return (true);
}

BOOL_TYPE(array_slimcheck)(ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  if (unlikely(_settled(self))) {
    return (false);
  }

  if (likely(_capacity(self))) {
    SIZE_TYPE(size) = array_sizeof(self);

    if (size < _capacity(self) / 2) {
      PTR_TYPE(ptr) = __array_allocator__._memory_realloc(_data(self), size);

      if (unlikely(!ptr)) {
        return (false);
      }

      _data(self) = ptr;
      _capacity(self) = size;
    }
  }

  return (true);
}

NONE_TYPE(array_settle)(ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  _settled(self) = true;
}

NONE_TYPE(array_unsettle)(ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);
  HR_COMPLAIN_IF(_is_owner(self) == false);

  _settled(self) = false;
}

__attr_pure BOOL_TYPE(array_is_settled)(RDONLY_ARRAY_TYPE(self)) {
  HR_COMPLAIN_IF(self == NULL);

  return (_settled(self));
}
