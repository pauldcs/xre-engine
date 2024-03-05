#include <stdint.h>

int32_t hash_string(const char *key)
{
    char *cp;
    int32_t k;

	cp = (char *)key;
	k = 0;
	while(*cp)
	    k = (((k << 1) + (k >> 14)) ^ (*cp++)) & 0x3fff;
	return(k);
}
