#ifndef __XRE_XDP__
#define __XRE_XDP__

#include "xre_core.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

size_t xre_xdp_ascii_16_bytes(uint8_t *dst, const uint8_t *src, size_t n);
size_t xre_xdp_data_16_bytes_color(uint8_t *buffer, const uint8_t *addr,
				   size_t n);
size_t xre_xdp_data_16_bytes(const uint8_t *dst, const uint8_t *src, size_t n);
size_t xre_xdp_pointer_8_bytes(uint8_t *dst, const uintptr_t p);

ssize_t xre_xdp_dump(const uint8_t *addr, size_t n, size_t offset);

#endif /* __XRE_XDP__ */
