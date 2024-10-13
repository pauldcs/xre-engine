#include "xre_xdp.h"
#include "xre_core.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

const uint32_t hex_char_pair_spaced[256] = {
	0x202e2e, 0x203130, 0x203230, 0x203330, 0x203430, 0x203530,
	0x203630, 0x203730, 0x203830, 0x203930, 0x206130, 0x206230,
	0x206330, 0x206430, 0x206530, 0x206630, 0x203031, 0x203131,
	0x203231, 0x203331, 0x203431, 0x203531, 0x203631, 0x203731,
	0x203831, 0x203931, 0x206131, 0x206231, 0x206331, 0x206431,
	0x206531, 0x206631, 0x203032, 0x203132, 0x203232, 0x203332,
	0x203432, 0x203532, 0x203632, 0x203732, 0x203832, 0x203932,
	0x206132, 0x206232, 0x206332, 0x206432, 0x206532, 0x206632,
	0x203033, 0x203133, 0x203233, 0x203333, 0x203433, 0x203533,
	0x203633, 0x203733, 0x203833, 0x203933, 0x206133, 0x206233,
	0x206333, 0x206433, 0x206533, 0x206633, 0x203034, 0x203134,
	0x203234, 0x203334, 0x203434, 0x203534, 0x203634, 0x203734,
	0x203834, 0x203934, 0x206134, 0x206234, 0x206334, 0x206434,
	0x206534, 0x206634, 0x203035, 0x203135, 0x203235, 0x203335,
	0x203435, 0x203535, 0x203635, 0x203735, 0x203835, 0x203935,
	0x206135, 0x206235, 0x206335, 0x206435, 0x206535, 0x206635,
	0x203036, 0x203136, 0x203236, 0x203336, 0x203436, 0x203536,
	0x203636, 0x203736, 0x203836, 0x203936, 0x206136, 0x206236,
	0x206336, 0x206436, 0x206536, 0x206636, 0x203037, 0x203137,
	0x203237, 0x203337, 0x203437, 0x203537, 0x203637, 0x203737,
	0x203837, 0x203937, 0x206137, 0x206237, 0x206337, 0x206437,
	0x206537, 0x206637, 0x203038, 0x203138, 0x203238, 0x203338,
	0x203438, 0x203538, 0x203638, 0x203738, 0x203838, 0x203938,
	0x206138, 0x206238, 0x206338, 0x206438, 0x206538, 0x206638,
	0x203039, 0x203139, 0x203239, 0x203339, 0x203439, 0x203539,
	0x203639, 0x203739, 0x203839, 0x203939, 0x206139, 0x206239,
	0x206339, 0x206439, 0x206539, 0x206639, 0x203061, 0x203161,
	0x203261, 0x203361, 0x203461, 0x203561, 0x203661, 0x203761,
	0x203861, 0x203961, 0x206161, 0x206261, 0x206361, 0x206461,
	0x206561, 0x206661, 0x203062, 0x203162, 0x203262, 0x203362,
	0x203462, 0x203562, 0x203662, 0x203762, 0x203862, 0x203962,
	0x206162, 0x206262, 0x206362, 0x206462, 0x206562, 0x206662,
	0x203063, 0x203163, 0x203263, 0x203363, 0x203463, 0x203563,
	0x203663, 0x203763, 0x203863, 0x203963, 0x206163, 0x206263,
	0x206363, 0x206463, 0x206563, 0x206663, 0x203064, 0x203164,
	0x203264, 0x203364, 0x203464, 0x203564, 0x203664, 0x203764,
	0x203864, 0x203964, 0x206164, 0x206264, 0x206364, 0x206464,
	0x206564, 0x206664, 0x203065, 0x203165, 0x203265, 0x203365,
	0x203465, 0x203565, 0x203665, 0x203765, 0x203865, 0x203965,
	0x206165, 0x206265, 0x206365, 0x206465, 0x206565, 0x206665,
	0x203066, 0x203166, 0x203266, 0x203366, 0x203466, 0x203566,
	0x203666, 0x203766, 0x203866, 0x203966, 0x206166, 0x206266,
	0x206366, 0x206466, 0x206566, 0x206666,
};

#define RED	0x00000001
#define GREEN	0x00000010
#define YELLOW	0x00000100
#define BLUE	0x00001000
#define MAGENTA 0x00010000
#define CYAN	0x00100000
#define WHITE	0x01000000
#define GREY	0x10000000

#define __ '.'

const char hex_dump_ascii[256] = {
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 ' ',  '!', '"', '#',
	'$', '%', '&', '\'', '(', ')', '*', '+', ',',  '-', '.', '/',
	'0', '1', '2', '3',  '4', '5', '6', '7', '8',  '9', ':', ';',
	'<', '=', '>', '?',  '@', 'A', 'B', 'C', 'D',  'E', 'F', 'G',
	'H', 'I', 'J', 'K',  'L', 'M', 'N', 'O', 'P',  'Q', 'R', 'S',
	'T', 'U', 'V', 'W',  'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
	'`', 'a', 'b', 'c',  'd', 'e', 'f', 'g', 'h',  'i', 'j', 'k',
	'l', 'm', 'n', 'o',  'p', 'q', 'r', 's', 't',  'u', 'v', 'w',
	'x', 'y', 'z', '{',  '|', '}', '~', __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __,   __,  __,  __,  __,	 __,   __,  __,	 __,
	__,  __,  __,  __
};

// ssize_t	xd_dump_lines(const ut8 *addr, size_t n, size_t offset, ut8
// *scr_ptr, size_t scr_size, bool color)
// {
// 	ut8 *prev = NULL;
// 	ut8 *ptr = (ut8 *)addr;
// 	size_t ret = 0;

// 	ut8 *scr_cursor = scr_ptr;

// 	bool dump_required = false;
// 	size_t line_size;

// 	while (n) {

// 		if (dump_required) {
// 			ret += xwrite(
// 				STDOUT_FILENO,
// 				scr_ptr,
// 				scr_cursor - scr_ptr);
// 			scr_cursor = scr_ptr;
// 			dump_required = false;
// 		}

// 		if (n < 16) {
// 			line_size = n;
// 			n = 0;

// 		} else {

// 			if (prev) {
// 				if (*(uint64_t *)(prev) == *(uint64_t *)(ptr) &&
// 					*(uint64_t *)(prev + 8) == *(uint64_t
// *)(ptr + 8)) { 					if (scr_cursor !=
// scr_ptr) {
// 						*(scr_cursor++) = '+';
// 						*(scr_cursor++) = '\n';
// 						dump_required = true;
// 					}
// 					offset += 16;
// 					ptr += 16;
// 					n -= 16;
// 					continue;
// 				}
// 			}
// 			line_size = 16;
// 			n -= 16;
// 		}

// 		scr_cursor += xd_pointer_8_bytes(scr_cursor, offset);
// 		*(scr_cursor++) = ' ';
// 		*(scr_cursor++) = ' ';
// 		if (color)
// 			scr_cursor += xd_data_16_bytes_color(scr_cursor, ptr,
// line_size); 		else 			scr_cursor +=
// xd_data_16_bytes(scr_cursor, ptr, line_size);
// 		*(scr_cursor++) = ' ';
// 		*(scr_cursor++) = ' ';
// 		scr_cursor += xd_ascii_16_bytes(scr_cursor, ptr, line_size);
// 		*(scr_cursor++) = '\n';

// 		if ((size_t)(scr_cursor - scr_ptr) >= scr_size)
// 			dump_required = true;

// 		prev = ptr;
// 		offset += 16;
// 		ptr += 16;
// 	}

// 	if ((size_t)(scr_cursor - scr_ptr))
// 		ret += xwrite(STDOUT_FILENO, scr_ptr, scr_cursor - scr_ptr);

// 	return(ret);
// }

ssize_t xre_xdp_dump(const uint8_t *addr, size_t n, size_t offset)
{
	const char buffer[512];
	uint8_t	  *ptr = (uint8_t *)addr;
	size_t	   ret = 0;
	size_t	   line_size;

	while (n) {
		char *tmp = (char *)buffer;
		if (n < 16) {
			line_size = n;
			n	  = 0;
		} else {
			line_size = 16;
			n -= 16;
		}
		tmp += xre_xdp_pointer_8_bytes((uint8_t *)tmp, offset);
		*(tmp++) = ' ';
		*(tmp++) = ' ';
		tmp += xre_xdp_data_16_bytes(
			(uint8_t *)tmp, ptr, line_size
		);
		*(tmp++) = ' ';
		*(tmp++) = ' ';
		tmp += xre_xdp_ascii_16_bytes(
			(uint8_t *)tmp, ptr, line_size
		);
		offset += 16;
		ptr += 16;
		write(STDOUT_FILENO, buffer, tmp - buffer);
		write(1, "\n", 1);
	}
	return (ret);
}

size_t
xre_xdp_ascii_16_bytes(uint8_t *dst, const uint8_t *src, size_t n)
{
	const uint32_t *src_u32 = (const uint32_t *)src;
	uint32_t       *dst_u32 = (uint32_t *)dst;

	size_t aligned = n & ~3;
	size_t ret     = 0;

	for (size_t i = 0; i < aligned; i += 4) {
		*dst_u32++ =
			hex_dump_ascii[*src_u32 & 0xff] |
			hex_dump_ascii[*src_u32 >> 8 & 0xff] << 8 |
			hex_dump_ascii[*src_u32 >> 16 & 0xff] << 16 |
			hex_dump_ascii[*src_u32 >> 24] << 24;
		src_u32++;
		ret += 4;
	}
	if (aligned != n) {
		const uint8_t *src_unaligned = src + aligned;
		uint8_t	      *dst_unaligned = dst + aligned;
		while (ret < n) {
			*dst_unaligned++ =
				hex_dump_ascii[*src_unaligned++];
			ret++;
		}
	}
	return (ret);
}

static const char __padding[] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

size_t
xre_xdp_data_16_bytes(const uint8_t *dst, const uint8_t *src, size_t n)
{
	switch (n) {
	case 16:
		for (size_t i = 0; i < n; i += 2, dst += 6) {
			*(uint32_t *)(dst) =
				hex_char_pair_spaced[src[i]];
			*(uint32_t *)(dst + 3) =
				hex_char_pair_spaced[src[i + 1]];
		}
		return (47);

	default:
		for (size_t i = 0; i < n; i++, dst += 3) {
			*(uint32_t *)(dst) =
				hex_char_pair_spaced[src[i]];
		}
	}
	xre_memcpy((void *)dst, __padding, (16 - n) * 3);
	return (47);
}

size_t xre_xdp_pointer_8_bytes(uint8_t *dst, uintptr_t p)
{
	static const char hex_chars[] = "0123456789abcdef";
	*dst++			      = '0';
	*dst++			      = 'x';
	for (int i = 7; i >= 0; --i) {
		dst[i] = hex_chars[p & 0xf];
		p >>= 4;
	}
	return (10);
}
