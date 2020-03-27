#include <string.h>
#include "hahash.h"

/* A simple hash function to encypt user's password tentatively. */
void hahash(const char *text, char *result)
{
	if (strlen(text) > hash::max_length) {
		memcpy(result, text, hash::max_length-1);
		result[hash::max_length-1] = 0;
		return;
	}

	strcpy(result, text);

	unsigned int start = 0;
	unsigned int len = strlen(text);

	for (int i=len; i<hash::max_length; i++, start++)
		result[i] = result[i-1] ^ result[start];

	start = hash::max_length - 1;
	for (int i=0; i<hash::max_length; i++) {
		result[i] ^= result[start-i];
		result[i] = hash::hash_table[result[i] % hash::hash_table_len];
	}

	result[hash::max_length-1] = 0;
}

