#ifndef __DYNSTR_H__
# define __DYNSTR_H__

# include <stddef.h>
# include <stdbool.h>

typedef struct {
    char  *_ptr;   /* data pointer */
    size_t _size;  /* size in use */
    size_t _cap;   /* allocated memory (in bytes) */
} dynstr_t;

/* Creates a new empty dynamic string and adjusts its starting capacity
 * to be at least enough to hold 'n' + 1 characters
 */
dynstr_t *dynstr_new (size_t n);

dynstr_t *dynstr_from (const char *src, size_t n);

/* Creates a string new dynamic string based on `src`.
 */
dynstr_t *dynstr_assign (const char *src);

/* Frees the allocated dynamic string
 */
void dynstr_kill (dynstr_t *self);

/* Appends the string pointed to by 'src' into the dynamic string `self`.
 */
bool dynstr_append (dynstr_t *self, const char *src);

/* Injects the string pointed to by 'src' into the dynamic string 'self', at
 * potitions 'p'.
 */
bool dynstr_inject (dynstr_t *self, const char *src, size_t p);

/* Sets the dynamic string `self` size to 0
 */
void dynstr_clear (dynstr_t *self);

/* Adjusts the dynamic string `self` capacity to be at least enough to
 * contain the current size + n characters.
 */
bool dynstr_adjust (dynstr_t *self, size_t n);

/* Removes all characters from the dynamic string within index 
 * start -> end
 */
void dynstr_wipe (dynstr_t *self, size_t start, size_t end);

size_t dynstr_cap (dynstr_t *self);

#endif /* __DYNSTR_H__ */
