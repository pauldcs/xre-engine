#include <string.h>

//  /path/foo.dylib		_debug   =>   /path/foo_debug.dylib	
//  foo.dylib			_debug   =>   foo_debug.dylib
//  foo     			_debug   =>   foo_debug
//  /path/bar			_debug   =>   /path/bar_debug  
//  /path/bar.A.dylib   _debug   =>   /path/bar.A_debug.dylib

void add_suffix(const char* path, const char* suffix, char* result)
{
	strcpy(result, path);
	
	char* start = strrchr(result, '/');
	if (start)
		start++;
	else
		start = result;
		
	char* dot = strrchr(start, '.');
	if (dot) {
		strcpy(dot, suffix);
		strcat(&dot[strlen(suffix)], &path[dot-result]);
	}
	else {
		strcat(result, suffix);
	}
}
