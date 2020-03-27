#ifndef __TERMINAL__HAHASH_H__
#define __TERMINAL__HAHASH_H__

namespace {
	namespace hash {
		unsigned int max_length = 32;
		unsigned int hash_table_len = 62;
		char hash_table[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', \
							'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', \
							'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', \
							'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', \
							'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', \
							'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	};
}

void hahash(const char *text, char *result);

#endif

