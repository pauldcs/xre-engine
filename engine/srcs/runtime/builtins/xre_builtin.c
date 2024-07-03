#include "xre_builtin.h"
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

builtin_func_t builtin_func_lookup[] = {
    {"print", builtin_print},
    {"debug", builtin_debug}
};

static ssize_t get_builtin_offset(const char *ptr, size_t size) {
  size_t lookup_size = sizeof(builtin_func_lookup) / sizeof (builtin_func_t);
  
  for (size_t i = 0; i < lookup_size; i++) {
    const char *name = builtin_func_lookup[i].name;
    if (strlen(name) == size) {
      if (!memcmp(name, ptr, size)) {
        return ((ssize_t)i);
      }
    }
  }
  return (-1);
}

bool is_defined_builtin(const char *ptr, size_t size) {
  return (get_builtin_offset(ptr, size) != -1);
}

builtin_func_ptr get_builtin_ptr(const char *ptr, size_t size) {
  ssize_t builtin_offset = get_builtin_offset(ptr, size);
  if (builtin_offset == -1) {
    return (NULL);
  }
  
  return (builtin_func_lookup[builtin_offset].func);
}
