#include "dstr.h"
#include "xre_compiler.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

// MODIFIED VERSION OF GIT's `strbuf`

static void die(const char *message)
{
	(void)fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

static int starts_with(const char *str, const char *prefix)
{
	for (;; str++, prefix++)
		if (!*prefix)
			return 1;
		else if (*str != *prefix)
			return 0;
}

/*
 * Used as the default ->buf value, so that people can always assume
 * buf is non NULL and ->buf is NUL terminated even for a freshly
 * initialized dstr.
 */
char dstr_slopbuf[1];

void dstr_init(dstr_t *self, size_t hint)
{
	__trigger_bug_if_fail(self);

	dstr_t blank = DSTR_INIT;
	(void)memcpy(self, &blank, sizeof(*self));

	if (hint) {
		dstr_grow(self, hint);
	}
}

void dstr_release(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	if (self->alloc) {
		free(self->buf);
		dstr_init(self, 0);
	}
}

char *dstr_detach(dstr_t *self, size_t *sz)
{
	__trigger_bug_if_fail(self);

	char *res = NULL;

	dstr_grow(self, 0);
	res = self->buf;

	if (sz) {
		*sz = self->len;
	}

	dstr_init(self, 0);
	return res;
}

void dstr_attach(dstr_t *self, void *buf, size_t len, size_t alloc)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(buf);

	dstr_release(self);

	self->buf   = buf;
	self->len   = len;
	self->alloc = alloc;

	dstr_grow(self, 0);
	self->buf[self->len] = '\0';
}

void dstr_grow(dstr_t *self, size_t extra)
{
	__trigger_bug_if_fail(self);

	int new_buf = !self->alloc;

	if (unsigned_add_overflows(extra, 1) ||
	    unsigned_add_overflows(self->len, extra + 1)) {
		die("you want to use way too much memory");
	}

	if (new_buf) {
		self->buf = NULL;
	}

	ALLOC_GROW(self->buf, self->len + extra + 1, self->alloc);

	if (new_buf) {
		self->buf[0] = '\0';
	}
}

void dstr_trim(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	dstr_rtrim(self);
	dstr_ltrim(self);
}

void dstr_rtrim(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	while (self->len > 0 &&
	       isspace((unsigned char)self->buf[self->len - 1])) {
		self->len--;
	}

	self->buf[self->len] = '\0';
}

static inline int is_dir_sep(int c)
{
	return c == '/';
}
void dstr_trim_trailing_dir_sep(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	while (self->len > 0 &&
	       is_dir_sep((unsigned char)self->buf[self->len - 1])) {
		self->len--;
	}

	self->buf[self->len] = '\0';
}

void dstr_trim_trailing_newline(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	if (self->len > 0 && self->buf[self->len - 1] == '\n') {
		if (--self->len > 0 &&
		    self->buf[self->len - 1] == '\r') {
			--self->len;
		}

		self->buf[self->len] = '\0';
	}
}

void dstr_ltrim(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	char *b = self->buf;
	while (self->len > 0 && isspace(*b)) {
		b++;
		self->len--;
	}

	(void)memmove(self->buf, b, self->len);
	self->buf[self->len] = '\0';
}

void dstr_tolower(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	char *p = self->buf, *end = self->buf + self->len;
	for (; p < end; p++) {
		*p = tolower(*p);
	}
}

dstr_t **
dstr_split_buf(const char *str, size_t slen, int terminator, int max)
{
	__trigger_bug_if_fail(str);

	dstr_t **ret = NULL;
	size_t	 nr = 0, alloc = 0;
	dstr_t	*t;

	while (slen) {
		int len = slen;
		if (max <= 0 || nr + 1 < (size_t)max) {
			const char *end =
				memchr(str, terminator, slen);
			if (end) {
				len = end - str + 1;
			}
		}

		t = malloc(sizeof(dstr_t));
		dstr_init(t, len);
		dstr_add(t, str, len);
		ALLOC_GROW(ret, nr + 2, alloc);
		ret[nr++] = t;
		str += len;
		slen -= len;
	}

	ALLOC_GROW(ret, nr + 1, alloc); /* In case string was empty */
	ret[nr] = NULL;
	return (ret);
}

int dstr_cmp(const dstr_t *a, const dstr_t *b)
{
	__trigger_bug_if_fail(a);
	__trigger_bug_if_fail(b);

	size_t len = a->len < b->len ? a->len : b->len;
	int    cmp = memcmp(a->buf, b->buf, len);

	if (cmp) {
		return (cmp);
	}
	return a->len < b->len ? -1 : a->len != b->len;
}

void dstr_splice(
	dstr_t	   *self,
	size_t	    pos,
	size_t	    len,
	const void *data,
	size_t	    dlen
)
{
	__trigger_bug_if_fail(self);

	if (unsigned_add_overflows(pos, len)) {
		die("you want to use way too much memory");
	}

	if (pos > self->len) {
		die("`pos' is too far after the end of the buffer");
	}

	if (pos + len > self->len) {
		die("`pos + len' is too far after the end of the buffer"
		);
	}

	if (dlen >= len) {
		dstr_grow(self, dlen - len);
	}

	(void)memmove(
		self->buf + pos + dlen,
		self->buf + pos + len,
		self->len - pos - len
	);
	(void)memcpy(self->buf + pos, data, dlen);

	dstr_setlen(self, self->len + dlen - len);
}

void dstr_insert(
	dstr_t *self, size_t pos, const void *data, size_t len
)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(data);

	dstr_splice(self, pos, 0, data, len);
}

void dstr_vinsertf(
	dstr_t *self, size_t pos, const char *fmt, va_list ap
)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(fmt);

	int	len, len2;
	char	save;
	va_list cp;

	if (pos > self->len) {
		die("`pos' is too far after the end of the buffer");
	}

	va_copy(cp, ap);
	len = vsnprintf(self->buf + self->len, 0, fmt, cp);
	va_end(cp);

	if (len < 0) {
		die("unable to format message");
	}

	if (!len) {
		return; /* nothing to do */
	}

	if (unsigned_add_overflows(self->len, (size_t)len)) {
		die("you want to use way too much memory");
	}

	dstr_grow(self, len);
	(void)memmove(
		self->buf + pos + len,
		self->buf + pos,
		self->len - pos
	);

	/* vsnprintf() will append a NUL, overwriting one of our characters */
	save = self->buf[pos + len];
	len2 = vsnprintf(self->buf + pos, len + 1, fmt, ap);
	self->buf[pos + len] = save;

	if (len2 != len) {
		die("your vsnprintf is broken (returns inconsistent lengths)"
		);
	}

	dstr_setlen(self, self->len + len);
}

void dstr_insertf(dstr_t *self, size_t pos, const char *fmt, ...)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(fmt);

	va_list ap;
	va_start(ap, fmt);
	dstr_vinsertf(self, pos, fmt, ap);
	va_end(ap);
}

void dstr_remove(dstr_t *self, size_t pos, size_t len)
{
	__trigger_bug_if_fail(self);

	dstr_splice(self, pos, len, "", 0);
}

void dstr_add(dstr_t *self, const void *data, size_t len)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(data);

	dstr_grow(self, len);
	(void)memcpy(self->buf + self->len, data, len);
	dstr_setlen(self, self->len + len);
}

void dstr_addstrings(dstr_t *self, const char *s, size_t n)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(s);

	size_t len = strlen(s);

	dstr_grow(self, st_mult(len, n));
	for (size_t i = 0; i < n; i++) {
		dstr_add(self, s, len);
	}
}

void dstr_addbuf(dstr_t *self, const dstr_t *other)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(other);

	dstr_grow(self, other->len);
	(void)memcpy(self->buf + self->len, other->buf, other->len);
	dstr_setlen(self, self->len + other->len);
}

const char *
dstr_join_argv(dstr_t *self, int argc, const char **argv, char delim)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(argv);

	if (!argc) {
		return self->buf;
	}

	dstr_addstr(self, *argv);
	while (--argc) {
		dstr_addch(self, delim);
		dstr_addstr(self, *(++argv));
	}

	return (self->buf);
}

void dstr_addchars(dstr_t *self, int c, size_t n)
{
	__trigger_bug_if_fail(self);

	dstr_grow(self, n);
	(void)memset(self->buf + self->len, c, n);
	dstr_setlen(self, self->len + n);
}

void dstr_addf(dstr_t *self, const char *fmt, ...)
{
	__trigger_bug_if_fail(self);

	va_list ap;
	va_start(ap, fmt);
	dstr_vaddf(self, fmt, ap);
	va_end(ap);
}

static void add_lines(
	dstr_t	   *out,
	const char *prefix,
	const char *buf,
	size_t	    size,
	int	    space_after_prefix
)
{
	while (size) {
		const char *next = memchr(buf, '\n', size);
		next		 = next ? (next + 1) : (buf + size);

		dstr_addstr(out, prefix);
		if (space_after_prefix && buf[0] != '\n' &&
		    buf[0] != '\t') {
			dstr_addch(out, ' ');
		}

		dstr_add(out, buf, next - buf);
		size -= next - buf;
		buf = next;
	}
	dstr_complete_line(out);
}

void dstr_vaddf(dstr_t *self, const char *fmt, va_list ap)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(fmt);

	int	len;
	va_list cp;

	if (!dstr_avail(self)) {
		dstr_grow(self, 64);
	}

	va_copy(cp, ap);
	len = vsnprintf(
		self->buf + self->len, self->alloc - self->len, fmt, cp
	);
	va_end(cp);
	if (len < 0) {
		die("unable to format message");
	}

	if ((size_t)len > dstr_avail(self)) {
		dstr_grow(self, len);
		len = vsnprintf(
			self->buf + self->len,
			self->alloc - self->len,
			fmt,
			ap
		);
		if ((size_t)len > dstr_avail(self)) {
			die("your vsnprintf is broken (insatiable)");
		}
	}

	dstr_setlen(self, self->len + len);
}

size_t dstr_fread(dstr_t *self, size_t size, FILE *f)
{
	__trigger_bug_if_fail(self);

	size_t res;
	size_t oldalloc = self->alloc;

	dstr_grow(self, size);
	res = fread(self->buf + self->len, 1, size, f);

	if (res > 0) {
		dstr_setlen(self, self->len + res);
	} else if (oldalloc == 0) {
		dstr_release(self);
	}

	return (res);
}

static ssize_t read_in_full(int fd, void *buf, size_t count)
{
	char   *p     = buf;
	ssize_t total = 0;

	while (count > 0) {
		ssize_t loaded = read(fd, p, count);
		if (loaded < 0)
			return -1;
		if (loaded == 0)
			return total;
		count -= loaded;
		p += loaded;
		total += loaded;
	}

	return total;
}
ssize_t dstr_read(dstr_t *self, int fd, size_t hint)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(fd != -1);

	size_t oldlen	= self->len;
	size_t oldalloc = self->alloc;

	dstr_grow(self, hint ? hint : 8192);

	for (;;) {
		ssize_t want = self->alloc - self->len - 1;
		ssize_t got =
			read_in_full(fd, self->buf + self->len, want);

		if (got < 0) {
			if (oldalloc == 0) {
				dstr_release(self);
			} else {
				dstr_setlen(self, oldlen);
			}
			return (-1);
		}
		self->len += got;
		if (got < want) {
			break;
		}

		dstr_grow(self, 8192);
	}

	self->buf[self->len] = '\0';
	return (self->len - oldlen);
}

ssize_t dstr_read_once(dstr_t *self, int fd, size_t hint)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(fd != -1);

	size_t	oldalloc = self->alloc;
	ssize_t count;

	dstr_grow(self, hint ? hint : 8192);
	count = read(
		fd, self->buf + self->len, self->alloc - self->len - 1
	);

	if (count > 0) {
		dstr_setlen(self, self->len + count);

	} else if (oldalloc == 0) {
		dstr_release(self);
	}

	return (count);
}

ssize_t dstr_write(dstr_t *self, FILE *f)
{
	__trigger_bug_if_fail(self);

	return self->len ? fwrite(self->buf, 1, self->len, f) : 0;
}

#define DSTR_MAXLINK (2 * PATH_MAX)

int dstr_readlink(dstr_t *self, const char *path, size_t hint)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(path);

	size_t oldalloc = self->alloc;

	if (hint < 32) {
		hint = 32;
	}

	while (hint < DSTR_MAXLINK) {
		ssize_t len;

		dstr_grow(self, hint);
		len = readlink(path, self->buf, hint);
		if (len < 0) {
			if (errno != ERANGE) {
				break;
			}
		} else if ((size_t)len < hint) {
			dstr_setlen(self, len);
			return (0);
		}

		/* .. the buffer was too small - try again */
		hint *= 2;
	}
	if (oldalloc == 0) {
		dstr_release(self);
	}

	return (-1);
}

int dstr_getcwd(dstr_t *self)
{
	__trigger_bug_if_fail(self);

	size_t oldalloc	   = self->alloc;
	size_t guessed_len = 128;

	for (;; guessed_len *= 2) {
		dstr_grow(self, guessed_len);
		if (getcwd(self->buf, self->alloc)) {
			dstr_setlen(self, strlen(self->buf));
			return 0;
		}

		/*
		 * If getcwd(3) is implemented as a syscall that falls
		 * back to a regular lookup using readdir(3) etc. then
		 * we may be able to avoid EACCES by providing enough
		 * space to the syscall as it's not necessarily bound
		 * to the same restrictions as the fallback.
		 */
		if (errno == EACCES && guessed_len < PATH_MAX) {
			continue;
		}

		if (errno != ERANGE) {
			break;
		}
	}
	if (oldalloc == 0) {
		dstr_release(self);

	} else {
		dstr_reset(self);
	}

	return (-1);
}

int dstr_getwholeline(dstr_t *self, FILE *fp, int term)
{
	__trigger_bug_if_fail(self);

	int ch;

	if (feof(fp)) {
		return (EOF);
	}

	dstr_reset(self);
	flockfile(fp);

	while ((ch = getc_unlocked(fp)) != EOF) {
		if (!dstr_avail(self)) {
			dstr_grow(self, 1);
		}

		self->buf[self->len++] = ch;
		if (ch == term) {
			break;
		}
	}

	funlockfile(fp);

	if (ch == EOF && self->len == 0) {
		return (EOF);
	}

	self->buf[self->len] = '\0';
	return (0);
}

int dstr_appendwholeline(dstr_t *self, FILE *fp, int term)
{
	__trigger_bug_if_fail(self);

	dstr_t line = DSTR_INIT;

	if (dstr_getwholeline(&line, fp, term)) {
		dstr_release(&line);
		return (EOF);
	}

	dstr_addbuf(self, &line);
	dstr_release(&line);

	return (0);
}

static int dstr_getdelim(dstr_t *self, FILE *fp, int term)
{
	if (dstr_getwholeline(self, fp, term)) {
		return (EOF);
	}

	if (self->buf[self->len - 1] == term) {
		dstr_setlen(self, self->len - 1);
	}

	return (0);
}

int dstr_getdelim_strip_crlf(dstr_t *self, FILE *fp, int term)
{
	__trigger_bug_if_fail(self);

	if (dstr_getwholeline(self, fp, term)) {
		return (EOF);
	}

	if (term == '\n' && self->buf[self->len - 1] == '\n') {
		dstr_setlen(self, self->len - 1);
		if (self->len && self->buf[self->len - 1] == '\r')
			dstr_setlen(self, self->len - 1);
	}

	return (0);
}

int dstr_getline(dstr_t *self, FILE *fp)
{
	__trigger_bug_if_fail(self);

	return (dstr_getdelim_strip_crlf(self, fp, '\n'));
}

int dstr_getline_lf(dstr_t *self, FILE *fp)
{
	__trigger_bug_if_fail(self);

	return (dstr_getdelim(self, fp, '\n'));
}

int dstr_getline_nul(dstr_t *self, FILE *fp)
{
	__trigger_bug_if_fail(self);

	return dstr_getdelim(self, fp, '\0');
}

int dstr_getwholeline_fd(dstr_t *self, int fd, int term)
{
	__trigger_bug_if_fail(self);

	dstr_reset(self);

	while (1) {
		char	ch;
		ssize_t len = read(fd, &ch, 1);
		if (len <= 0) {
			return EOF;
		}

		dstr_addch(self, ch);
		if (ch == term) {
			break;
		}
	}

	return (0);
}

ssize_t dstr_read_file(dstr_t *self, const char *path, size_t hint)
{
	__trigger_bug_if_fail(self);
	__trigger_bug_if_fail(path);

	int	fd;
	ssize_t len;
	int	saved_errno;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (-1);

	len	    = dstr_read(self, fd, hint);
	saved_errno = errno;

	(void)close(fd);

	if (len < 0) {
		errno = saved_errno;
		return (-1);
	}

	return (len);
}

void dstr_add_lines(
	dstr_t *self, const char *prefix, const char *buf, size_t size
)
{
	__trigger_bug_if_fail(self);

	add_lines(self, prefix, buf, size, 0);
}

/*
 * Returns the length of a line, without trailing spaces.
 *
 * If the line ends with newline, it will be removed too.
 */
static size_t cleanup(char *line, size_t len)
{
	while (len) {
		unsigned char c = line[len - 1];
		if (!isspace(c))
			break;
		len--;
	}

	return len;
}

/*
 * Remove empty lines from the beginning and end
 * and also trailing spaces from every line.
 *
 * Turn multiple consecutive empty lines between paragraphs
 * into just one empty line.
 *
 * If the input has only empty lines and spaces,
 * no output will be produced.
 *
 * If last line does not have a newline at the end, one is added.
 *
 * Pass a non-NULL comment_prefix to skip every line starting
 * with it.
 */
void dstr_stripspace(dstr_t *self, const char *comment_prefix)
{
	size_t empties = 0;
	size_t i, j, len, newlen;
	char  *eol;

	/* We may have to add a newline. */
	dstr_grow(self, 1);

	for (i = j = 0; i < self->len; i += len, j += newlen) {
		eol = memchr(self->buf + i, '\n', self->len - i);
		len = eol ? eol - (self->buf + i) + 1 : self->len - i;

		if (comment_prefix && len &&
		    starts_with(self->buf + i, comment_prefix)) {
			newlen = 0;
			continue;
		}
		newlen = cleanup(self->buf + i, len);

		/* Not just an empty line? */
		if (newlen) {
			if (empties > 0 && j > 0) {
				self->buf[j++] = '\n';
			}

			empties = 0;
			(void)memmove(
				self->buf + j, self->buf + i, newlen
			);
			self->buf[newlen + j++] = '\n';

		} else {
			empties++;
		}
	}

	dstr_setlen(self, j);
}

static inline char *find_last_dir_sep(const char *path)
{
	return strrchr(path, '/');
}
void dstr_strip_file_from_path(dstr_t *self)
{
	char *path_sep = find_last_dir_sep(self->buf);
	dstr_setlen(self, path_sep ? path_sep - self->buf + 1 : 0);
}
