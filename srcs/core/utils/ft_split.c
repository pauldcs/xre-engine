#include "xleaks.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static void	free_2d(char **arr)
{
	size_t	idx = 0;

	while (arr[idx])
		__xfree__(arr[idx++]);
	__xfree__(arr);
}

static inline bool	is_separator(int c, char *sep)
{
	while (*sep)
	{
		if (c == *sep)
			return (true);
		sep++;
	}
	return (false);
}

static size_t	count_tokens(char const *str, char *sep)
{
	size_t	count = 0;

	while (*str)
	{
		while (*str && is_separator(*str, sep))
			str++;
		if (*str)
			count++;
		while (*str && !is_separator(*str, sep))
			str++;
	}
	return (count);
}

char	**ft_split(char const *str, char *sep) __attribute__((nonnull(str)))
{
	size_t	size = count_tokens(str, sep);
	char	**split;
	char 	**tmp = split;
	size_t	i;

	split = __xmalloc__(sizeof(char *) * (size + 1));
	if (!split)
		return (NULL);
	while (*str && is_separator(*str, sep))
		str++;
	while (*str) {
		i = 0;
		while (str[i] && (!is_separator(str[i], sep)))
			i++;
		if (i) {
			if ((*(split++) = strndup(str, i)) == NULL) {
				free_2d(tmp);
				return (NULL);
			}
			str += i;
			split++;
		}
		else
			str++;
	}
	*split = NULL;
	return (split - size);
}