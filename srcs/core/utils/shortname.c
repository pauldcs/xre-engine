#include <string.h>

const char *
shortname(const char *name) {

	if (name) {
		const char* s = strrchr(name, '/');
		if (s) 
			return &s[1];
	}
	return name; 
}
