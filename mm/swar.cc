unsigned int swar(unsigned int n)
{
	n = (n & 0x55555555) + ((n >> 1) & 0x55555555);

	n = (n & 0x33333333) + ((n >> 2) & 0x33333333);

	n = (n & 0x0f0f0f0f) + ((n >> 4) & 0x0f0f0f0f);

	n = (n * 0x01010101) >> 24;

	return n;
}

