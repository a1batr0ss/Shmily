void memcpy(char *dst, const char *src, unsigned int cnt)
{
    while (cnt-- > 0)
        *dst++ = *src++;
}

void memset(char *dst, unsigned char val, unsigned int cnt)
{
    while (cnt-- > 0)
        *dst++ = val;
}

void strcpy(char *dst, const char *src)
{
	while (*src)
		*dst++ = *src++;
}

unsigned int strlen(const char *str)
{
	unsigned int l = 0;
	while (*str++)
		l++;
	return l;
}

bool strcmp(const char *str1, const char *str2)
{
	while ((*str1) && (*str2) && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	if (!(*str1) && (!*str2))
		return true;
	return false;
}

