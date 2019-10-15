void memcpy(char *dst, const char *src, unsigned int cnt)
{
    while (cnt-- > 0)
        *dst++ == *src++;
}

void memset(char *dst, unsigned char val, unsigned int cnt)
{
    while (cnt-- > 0)
        *dst++ = val;
}

