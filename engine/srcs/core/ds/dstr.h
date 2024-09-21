#ifndef __DSTR_H__
#define __DSTR_H__

#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

/**
 * strbufs are meant to be used with all the usual C string and memory
 * APIs. Given that the length of the buffer is known, it's often better to
 * use the mem* functions than a str* one (e.g., memchr vs. strchr).
 * Though, one has to be careful about the fact that str* functions often
 * stop on NULs and that strbufs may have embedded NULs.
 *
 * A strbuf is NUL terminated for convenience, but no function in the
 * strbuf API actually relies on the string being free of NULs.
 *
 * strbufs have some invariants that are very important to keep in mind:
 *
 *  - The `buf` member is never NULL, so it can be used in any usual C
 *    string operations safely. strbufs _have_ to be initialized either by
 *    `strbuf_init()` or by `= STRBUF_INIT` before the invariants, though.
 *
 *    Do *not* assume anything on what `buf` really is (e.g. if it is
 *    allocated memory or not), use `strbuf_detach()` to unwrap a memory
 *    buffer from its strbuf shell in a safe way. That is the sole supported
 *    way. This will give you a malloced buffer that you can later `free()`.
 *
 *    However, it is totally safe to modify anything in the string pointed by
 *    the `buf` member, between the indices `0` and `len-1` (inclusive).
 *
 *  - The `buf` member is a byte array that has at least `len + 1` bytes
 *    allocated. The extra byte is used to store a `'\0'`, allowing the
 *    `buf` member to be a valid C-string. All strbuf functions ensure this
 *    invariant is preserved.
 *
 *    NOTE: It is OK to "play" with the buffer directly if you work it this
 *    way:
 *
 *        strbuf_grow(sb, SOME_SIZE); <1>
 *        strbuf_setlen(sb, sb->len + SOME_OTHER_SIZE);
 *
 *    <1> Here, the memory array starting at `sb->buf`, and of length
 *    `strbuf_avail(sb)` is all yours, and you can be sure that
 *    `strbuf_avail(sb)` is at least `SOME_SIZE`.
 *
 *    NOTE: `SOME_OTHER_SIZE` must be smaller or equal to `strbuf_avail(sb)`.
 *
 *    Doing so is safe, though if it has to be done in many places, adding the
 *    missing API to the strbuf module is the way to go.
 *
 *    WARNING: Do _not_ assume that the area that is yours is of size `alloc
 *    - 1` even if it's true in the current implementation. Alloc is somehow a
 *    "private" member that should not be messed with. Use `strbuf_avail()`
 *    instead.
*/

/**
 * Data Structures
 * ---------------
 */

/**
 * This is the string buffer structure. The `len` member can be used to
 * determine the current length of the string, and `buf` member provides
 * access to the string itself.
 */
typedef struct {
	char  *buf;
	size_t alloc;
	size_t len;
} dstr_t;

extern char dstr_slopbuf[];
#define DSTR_INIT                   \
	{                           \
		.buf = dstr_slopbuf \
	}

#define alloc_nr(x) (((x) + 16) * 3 / 2)
#define REALLOC_ARRAY(x, alloc) \
	(x) = realloc((x), st_mult(sizeof(*(x)), (alloc)))

#define ALLOC_GROW(x, nr, alloc)                         \
	do {                                             \
		if ((nr) > alloc) {                      \
			if (alloc_nr(alloc) < (nr))      \
				alloc = (nr);            \
			else                             \
				alloc = alloc_nr(alloc); \
			REALLOC_ARRAY(x, alloc);         \
		}                                        \
	} while (0)

/**
 * Life Cycle Functions
 * --------------------
 */

/**
 * Initialize the structure. The second parameter can be zero or a bigger
 * number to allocate memory, in case you want to prevent further reallocs.
 */
void dstr_init(dstr_t *self, size_t alloc);

/**
 * Release a string buffer and the memory it used. After this call, the
 * dstr points to an empty string that does not need to be free()ed, as
 * if it had been set to `DSTR_INIT` and never modified.
 *
 * To clear a strbuf in preparation for further use without the overhead
 * of free()ing and malloc()ing again, use dstr_reset() instead.
 */
void dstr_release(dstr_t *self);

/**
 * Detach the string from the dstr and returns it; you now own the
 * storage the string occupies and it is your responsibility from then on
 * to release it with `free(3)` when you are done with it.
 *
 * The dstr that previously held the string is reset to `DSTR_INIT` so
 * it can be reused after calling this function.
 */
char *dstr_detach(dstr_t *self, size_t *sz);

/**
 * Attach a string to a buffer. You should specify the string to attach,
 * the current length of the string and the amount of allocated memory.
 * The amount must be larger than the string length, because the string you
 * pass is supposed to be a NUL-terminated string.  This string _must_ be
 * malloc()ed, and after attaching, the pointer cannot be relied upon
 * anymore, and neither be free()d directly.
 */
void dstr_attach(dstr_t *self, void *str, size_t len, size_t mem);

/**
 * Functions related to the size of the buffer
 * -------------------------------------------
 */

/**
 * Determine the amount of allocated but unused memory.
 */
static inline size_t dstr_avail(dstr_t *self)
{
	return (self->alloc ? self->alloc - self->len - 1 : 0);
}

/**
 * Ensure that at least this amount of unused memory is available after
 * `len`. This is used when you know a typical size for what you will add
 * and want to avoid repetitive automatic resizing of the underlying buffer.
 * This is never a needed operation, but can be critical for performance in
 * some cases.
 */
void dstr_grow(dstr_t *self, size_t amount);

/**
 * Set the length of the buffer to a given value. This function does *not*
 * allocate new memory, so you should not perform a `dstr_setlen()` to a
 * length that is larger than `len + dstr_avail()`. `dstr_setlen()` is
 * just meant as a 'please fix invariants from this dstr I just messed
 * with'.
 */
static inline void dstr_setlen(dstr_t *self, size_t len)
{
	if (len > (self->alloc ? self->alloc - 1 : 0)) {
		assert(0 && "strbuf_setlen() beyond buffer");
	}

	self->len = len;
	if (self->buf != dstr_slopbuf) {
		self->buf[len] = '\0';
	} else {
		assert(!dstr_slopbuf[0]);
	}
}

/**
 * Empty the buffer by setting the size of it to zero.
 */
#define dstr_reset(self) dstr_setlen(self, 0)

/**
 * Functions related to the contents of the buffer
 * -----------------------------------------------
 */

/**
 * Strip whitespace from the beginning (`ltrim`), end (`rtrim`), or both side
 * (`trim`) of a string.
 */
void dstr_trim(dstr_t *self);
void dstr_rtrim(dstr_t *self);
void dstr_ltrim(dstr_t *self);

/* Strip trailing directory separators */
void dstr_trim_trailing_dir_sep(dstr_t *self);

/* Strip trailing LF or CR/LF */
void dstr_trim_trailing_newline(dstr_t *self);

/**
 * Lowercase each character in the buffer using `tolower`.
 */
void dstr_tolower(dstr_t *self);

/**
 * Compare two buffers. Returns an integer less than, equal to, or greater
 * than zero if the first buffer is found, respectively, to be less than,
 * to match, or be greater than the second buffer.
 */
int dstr_cmp(const dstr_t *a, const dstr_t *b);

/**
 * Adding data to the buffer
 * -------------------------
 *
 * NOTE: All of the functions in this section will grow the buffer as
 * necessary.  If they fail for some reason other than memory shortage and the
 * buffer hadn't been allocated before (i.e. the `struct strbuf` was set to
 * `DSTR_INIT`), then they will free() it.
 */

/**
 * Add a single character to the buffer.
 */
static inline void dstr_addch(dstr_t *self, int c)
{
	if (!dstr_avail(self)) {
		dstr_grow(self, 1);
	}

	self->buf[self->len++] = c;
	self->buf[self->len]   = '\0';
}

/**
 * Add a character the specified number of times to the buffer.
 */
void dstr_addchars(dstr_t *self, int c, size_t n);

/**
 * Insert data to the given position of the buffer. The remaining contents
 * will be shifted, not overwritten.
 */
void dstr_insert(dstr_t *self, size_t pos, const void *, size_t);

/**
 * Insert a NUL-terminated string to the given position of the buffer.
 * The remaining contents will be shifted, not overwritten.  It's an
 * inline function to allow the compiler to resolve strlen() calls on
 * constants at compile time.
 */
static inline void
dstr_insertstr(dstr_t *self, size_t pos, const char *s)
{
	dstr_insert(self, pos, s, strlen(s));
}

/**
 * Insert data to the given position of the buffer giving a printf format
 * string. The contents will be shifted, not overwritten.
 */
void dstr_vinsertf(
	dstr_t *self, size_t pos, const char *fmt, va_list ap
);

__attribute__((format(printf, 3, 4))) void
dstr_insertf(dstr_t *self, size_t pos, const char *fmt, ...);

/**
 * Remove given amount of data from a given position of the buffer.
 */
void dstr_remove(dstr_t *self, size_t pos, size_t len);

/**
 * Remove the bytes between `pos..pos+len` and replace it with the given
 * data.
 */
void dstr_splice(
	dstr_t	   *self,
	size_t	    pos,
	size_t	    len,
	const void *data,
	size_t	    data_len
);

/**
 * Add a NUL-terminated string to the buffer. Each line will be prepended
 * by a comment character and a blank.
 */
void dstr_add_commented_lines(
	dstr_t	   *self,
	const char *buf,
	size_t	    size,
	const char *comment_prefix
);

/**
 * Add data of given length to the buffer.
 */
void dstr_add(dstr_t *self, const void *data, size_t len);

/**
 * Add a NUL-terminated string to the buffer.
 *
 * NOTE: This function will *always* be implemented as an inline or a macro
 * using strlen, meaning that this is efficient to write things like:
 *
 *     dstr_addstr(sb, "immediate string");
 *
 */
static inline void dstr_addstr(dstr_t *self, const char *s)
{
	dstr_add(self, s, strlen(s));
}

/**
 * Add a NUL-terminated string the specified number of times to the buffer.
 */
void dstr_addstrings(dstr_t *self, const char *s, size_t n);

/**
 * Copy the contents of another buffer at the end of the current one.
 */
void dstr_addbuf(dstr_t *self, const dstr_t *other);

/**
 * Join the arguments into a buffer. `delim` is put between every
 * two arguments.
 */
const char *
dstr_join_argv(dstr_t *self, int argc, const char **argv, char delim);

#define DSTR_ENCODE_SLASH 1

/**
 * Append the contents of a string to a dstr, percent-encoding any characters
 * that are needed to be encoded for a URL.
 *
 * If DSTR_ENCODE_SLASH is set in flags, percent-encode slashes.  Otherwise,
 * slashes are not percent-encoded.
 */
void dstr_add_percentencode(dstr_t *dst, const char *src, int flags);

/**
 * Add a formatted string to the buffer.
 */
__attribute__((format(printf, 2, 3))) void
dstr_addf(dstr_t *self, const char *fmt, ...);

/**
 * Add a formatted string prepended by a comment character and a
 * blank to the buffer.
 */
__attribute__((format(printf, 3, 4))) void dstr_commented_addf(
	dstr_t *self, const char *comment_prefix, const char *fmt, ...
);

__attribute__((format(printf, 2, 0))) void
dstr_vaddf(dstr_t *self, const char *fmt, va_list ap);

/**
 * Read a given size of data from a FILE* pointer to the buffer.
 *
 * NOTE: The buffer is rewound if the read fails. If -1 is returned,
 * `errno` must be consulted, like you would do for `read(3)`.
 * `dstr_read()`, `dstr_read_file()` and `dstr_getline_*()`
 * family of functions have the same behaviour as well.
 */
size_t dstr_fread(dstr_t *self, size_t size, FILE *file);

/**
 * Read the contents of a given file descriptor. The third argument can be
 * used to give a hint about the file size, to avoid reallocs.  If read fails,
 * any partial read is undone.
 */
ssize_t dstr_read(dstr_t *self, int fd, size_t hint);

/**
 * Read the contents of a given file descriptor partially by using only one
 * attempt of xread. The third argument can be used to give a hint about the
 * file size, to avoid reallocs. Returns the number of new bytes appended to
 * the sb.
 */
ssize_t dstr_read_once(dstr_t *self, int fd, size_t hint);

/**
 * Read the contents of a file, specified by its path. The third argument
 * can be used to give a hint about the file size, to avoid reallocs.
 * Return the number of bytes read or a negative value if some error
 * occurred while opening or reading the file.
 */
ssize_t dstr_read_file(dstr_t *self, const char *path, size_t hint);

/**
 * Read the target of a symbolic link, specified by its path.  The third
 * argument can be used to give a hint about the size, to avoid reallocs.
 */
int dstr_readlink(dstr_t *self, const char *path, size_t hint);

/**
 * Write the whole content of the strbuf to the stream not stopping at
 * NUL bytes.
 */
ssize_t dstr_write(dstr_t *self, FILE *stream);

/**
 * Read from a FILE * until the specified terminator is encountered,
 * overwriting the existing contents of the dstr.
 *
 * Reading stops after the terminator or at EOF.  The terminator is
 * removed from the buffer before returning.  If the terminator is LF
 * and if it is preceded by a CR, then the whole CRLF is stripped.
 * Returns 0 unless there was nothing left before EOF, in which case
 * it returns `EOF`.
 */
int dstr_getdelim_strip_crlf(dstr_t *self, FILE *fp, int term);

/**
 * Read a line from a FILE *, overwriting the existing contents of
 * the dstr.  The dstr_getline*() family of functions share
 * this signature, but have different line termination conventions.
 *
 * Reading stops after the terminator or at EOF.  The terminator
 * is removed from the buffer before returning.  Returns 0 unless
 * there was nothing left before EOF, in which case it returns `EOF`.
 */
typedef int (*dstr_getline_fn)(dstr_t *, FILE *);

/* Uses LF as the line terminator */
int dstr_getline_lf(dstr_t *self, FILE *fp);

/* Uses NUL as the line terminator */
int dstr_getline_nul(dstr_t *self, FILE *fp);

/*
 * Similar to dstr_getline_lf(), but additionally treats a CR that
 * comes immediately before the LF as part of the terminator.
 * This is the most friendly version to be used to read "text" files
 * that can come from platforms whose native text format is CRLF
 * terminated.
 */
int dstr_getline(dstr_t *self, FILE *file);

/**
 * Like `dstr_getline`, but keeps the trailing terminator (if
 * any) in the buffer.
 */
int dstr_getwholeline(dstr_t *self, FILE *file, int term);

/**
 * Like `dstr_getwholeline`, but appends the line instead of
 * resetting the buffer first.
 */
int dstr_appendwholeline(dstr_t *self, FILE *file, int term);

/**
 * Like `dstr_getwholeline`, but operates on a file descriptor.
 * It reads one character at a time, so it is very slow.  Do not
 * use it unless you need the correct position in the file
 * descriptor.
 */
int dstr_getwholeline_fd(dstr_t *self, int fd, int term);

/**
 * Set the buffer to the path of the current working directory.
 */
int dstr_getcwd(dstr_t *self);

/**
 * Strip whitespace from a buffer. If comment_prefix is non-NULL,
 * then lines beginning with that character are considered comments,
 * thus removed.
 */
void dstr_stripspace(dstr_t *self, const char *comment_prefix);

static inline bool
strip_suffix_mem(const char *buf, size_t *len, const char *suffix)
{
	size_t suflen = strlen(suffix);
	if (*len < suflen ||
	    memcmp(buf + (*len - suflen), suffix, suflen))
		return false;
	*len -= suflen;
	return true;
}

static inline int dstr_strip_suffix(dstr_t *self, const char *suffix)
{
	if (strip_suffix_mem(self->buf, &self->len, suffix)) {
		dstr_setlen(self, self->len);
		return (1);
	} else
		return (0);
}

/**
 * Split str (of length slen) at the specified terminator character.
 * Return a null-terminated array of pointers to dstr objects
 * holding the substrings.  The substrings include the terminator,
 * except for the last substring, which might be unterminated if the
 * original string did not end with a terminator.  If max is positive,
 * then split the string into at most max substrings (with the last
 * substring containing everything following the (max-1)th terminator
 * character).
 *
 * The most generic form is `dstr_split_buf`, which takes an arbitrary
 * pointer/len buffer. The `_str` variant takes a NUL-terminated string,
 * the `_max` variant takes a dstr, and just `dstr_split` is a convenience
 * wrapper to drop the `max` parameter.
 *
 * For lighter-weight alternatives, see string_list_split() and
 * string_list_split_in_place().
 */
dstr_t **
dstr_split_buf(const char *str, size_t len, int terminator, int max);

static inline dstr_t **
dstr_split_str(const char *str, int terminator, int max)
{
	return dstr_split_buf(str, strlen(str), terminator, max);
}

static inline dstr_t **
dstr_split_max(const dstr_t *self, int terminator, int max)
{
	return dstr_split_buf(self->buf, self->len, terminator, max);
}

static inline dstr_t **dstr_split(const dstr_t *self, int terminator)
{
	return dstr_split_max(self, terminator, 0);
}

/*
 * Remove the filename from the provided path string. If the path
 * contains a trailing separator, then the path is considered a directory
 * and nothing is modified.
 *
 * Examples:
 * - "/path/to/file" -> "/path/to/"
 * - "/path/to/dir/" -> "/path/to/dir/"
 */
void dstr_strip_file_from_path(dstr_t *self);

void dstr_add_lines(
	dstr_t *self, const char *prefix, const char *buf, size_t size
);

/**
 * "Complete" the contents of `self` by ensuring that either it ends with the
 * character `term`, or it is empty.  This can be used, for example,
 * to ensure that text ends with a newline, but without creating an empty
 * blank line if there is no content in the first place.
 */
static inline void dstr_complete(dstr_t *self, char term)
{
	if (self->len && self->buf[self->len - 1] != term)
		dstr_addch(self, term);
}

static inline void dstr_complete_line(dstr_t *self)
{
	dstr_complete(self, '\n');
}

__attribute__((format(printf, 1, 2))) int
printf_ln(const char *fmt, ...);
__attribute__((format(printf, 2, 3))) int
fprintf_ln(FILE *fp, const char *fmt, ...);

static inline bool
strip_suffix(const char *str, const char *suffix, size_t *len)
{
	*len = strlen(str);
	return strip_suffix_mem(str, len, suffix);
}

static inline int ends_with(const char *str, const char *suffix)
{
	size_t len;
	return strip_suffix(str, suffix, &len);
}

#endif /* __DSTR_H__ */
