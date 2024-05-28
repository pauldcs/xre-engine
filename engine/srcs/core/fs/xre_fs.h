#ifndef __XRE_FS_H__
#define __XRE_FS_H__

#include "xre_core.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef struct s_xre_fs_file {
  const char *name;
  const char *path;
  void *ptr;
  off_t size;
  uint64_t sof;
  uint64_t eof;
} t_xre_fs_file;

bool xre_file_exists(const char *fn);
bool xre_file_is_dir(const char *fn);
bool xre_file_is_regular(const char *fn);
bool xre_file_get_size(const char *filename, size_t *size);

bool xre_fd_is_block_device(const int fd);
bool xre_fd_is_character_device(const int fd);
bool xre_fd_is_pipe(const int fd);
bool xre_fd_is_readable(const int fd);
bool xre_fd_is_regular(const int fd);
bool xre_fd_is_valid(const int fd);
bool xre_fd_is_writable(const int fd);

bool xre_file_open_read(const char *fn, int *fd);
bool xre_file_open_append(const char *fn, int *fd);
bool xre_file_open_write(const char *fn, int *fd);
bool xre_fd_read_at(int fd, void *dest, size_t n, off_t offset);
bool xre_fd_sneek_read(int fd, void *dest, size_t n);
bool xre_fd_read(int fd, void *dest, size_t n);

void xre_file_print(struct stat *sb);

#endif /* __XRE_FS_H__ */
