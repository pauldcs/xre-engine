// #include <stdbool.h>

// bool strpcmp(char *str, char *p)
// {
// 	if (*p == '\0')
// 		return *str == '\0';

// 	while (*str != '\0' && *p != '\0') {
// 		if (*p == '*') {
// 			do {
// 				if (strpcmp(str, p + 1))
// 					return (true);
// 			} while (*str++ != '\0');
// 			return (false);
// 		}

// 		if (*p == '?') {
// 			if (*str == '\0')
// 				return (false);

// 			p++;
// 			str++;
// 			continue;
// 		}

// 		if (*str != *p)
// 			return (false);

// 		p++;
// 		str++;
// 	}

// 	if (*str == '\0') {
// 		while (*p == '*')
// 			p++;

// 		return *p == '\0';
// 	}
// 	return (false);
// }
