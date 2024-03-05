// #include "dynstr.h"
// #include <limits.h>
// #include <stdbool.h>
// #include <stddef.h>

// dynstr_t *dynstr_create(size_t n) {
//   HR_COMPLAIN_IF(LONG_MAX - 1 <= n);

//   array_t *dynstr = array_create(sizeof(char), n + 1, NULL);

//   if (likely(dynstr)) {
//     *(char *)dynstr->_ptr = '\0';
//     dynstr->_nmemb = 1;
//   }
//   return ((dynstr_t *)dynstr);
// }

// dynstr_t *dynstr_assign(const char *src, st64_t n) {
//   HR_COMPLAIN_IF(src == NULL);
//   HR_COMPLAIN_IF(LONG_MAX - 1 <= n);

//   size_t init_size = (n == -1) ? strlen(src) : n;
//   array_t *dynstr = array_create(sizeof(char), init_size + 1, NULL);

//   if (likely(dynstr)) {
//     (void)memmove(dynstr->_ptr, src, init_size);
//     *((char *)dynstr->_ptr + init_size) = '\0';
//     dynstr->_nmemb = init_size + 1;
//   }

//   return ((dynstr_t *)dynstr);
// }

// void dynstr_kill(dynstr_t *self) { array_kill((array_t *)self); }

// bool dynstr_append(dynstr_t *self, const char *str, st64_t n) {
//   HR_COMPLAIN_IF(str == NULL);
//   HR_COMPLAIN_IF(LONG_MAX - 1 <= n);

//   size_t str_length = (n == -1) ? strlen(str) : n;
//   return (array_inject((array_t *)self, self->_nmemb - 1, str, str_length));
// }

// bool dynstr_inject(dynstr_t *self, size_t pos, const char *src, st64_t n) {
//   HR_COMPLAIN_IF(src == NULL);
//   HR_COMPLAIN_IF(LONG_MAX - 1 <= n);

//   size_t str_length = (n == -1) ? strlen(src) : n;
//   return (array_inject((array_t *)self, pos, src, str_length));
// }

// void dynstr_wipe(dynstr_t *self, size_t start, size_t end) {
//   array_wipe((array_t *)self, start, end);
// }

// void dynstr_done(dynstr_t *self) { array_settle((array_t *)self); }

// dynstr_t *dynstr_pull(const dynstr_t *self, st64_t sp, st64_t ep);
// void dynstr_clear(dynstr_t *self);
// size_t dynstr_cap(const dynstr_t *self);
