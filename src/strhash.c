// #include "strhash.h"

// #define M (unsigned long long) 0xFFFFFFFFFFFFFFFF
// #define P 35

// hash_t strhash(const char *string)
// {
// 	hash_t hash = 1;
// 	register p_pow = P;
// 	for(int n = 0; string[n] != '\0'; n++)
// 	{
// 		hash = (hash * string[n] * M) * p_pow;
// 		p_pow *= p_pow;
// 	}
// 	return hash;
// }
#include <stdio.h>

void test()
{
	
}