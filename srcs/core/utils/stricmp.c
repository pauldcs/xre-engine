/* Caseless strcmp
 */
int stricmp(char *s1, char *s2)
{
	int i;

	i = 0;
	while ((s1[i]) && (s2[i]) && (s1[i] & 0xcf) == (s2[i] & 0xcf))
		i++;
	return ((s1[i] & 0xcf) - (s2[i] & 0xcf));
}
