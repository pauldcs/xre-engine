#include "array.h"
#include "xre_assert.h"
#include "xre_log.h"
#include "xre_parse.h"
#include "xre_runtime.h"

array_t *symtab = NULL;

bool symtab_init(void) {
  if (!symtab) {
    symtab = array_create(sizeof(symtab_entry_t), 8, NULL);
    if (!symtab)
      return (false);
  }
  return (true);
}

void symtab_deinit(void) {
  array_kill(symtab);
  symtab = NULL;
}

symtab_entry_t *symtab_get(const char *key) {
  size_t size = array_size(symtab);
  size_t i = 0;
  while (i < size) {
    symtab_entry_t *item = (symtab_entry_t *)array_at(symtab, i);
    if (strcmp(item->key, key) == 0) {
      return (item);
    }
    i++;
  }
  return (NULL);
}

static bool symtab_add(const char *key, state_t state) {
  __return_val_if_fail__(key, false);

  symtab_entry_t item;
  (void)memset(&item, 0, sizeof(symtab_entry_t));

  item.key = key;
  item.state = state;

  if (!array_push(symtab, &item)) {
    XRE_LOGGER(error, "Out of memory");
    exit(1);
  }

  return (true);
}

bool symtab_set(xre_frame_t *frame, const char *key, state_t state) {
  __return_val_if_fail__(key, false);
  __return_val_if_fail__(frame, false);
  
  symtab_entry_t *override = symtab_get(key);
  if (!override) {
    return (symtab_add(key, state));
  }

  (void)memmove(&override->state, &state, sizeof(state_t));

  return (true);
}
