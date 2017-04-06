#include "common.h"


/**
 * Compute the smallest divisor of a number.
 *   @n: The number
 *   &returns: The divisor, or zero if prime.
 */
uint64_t m_getdiv64(uint64_t n)
{
	uint64_t i;

	if((n & 1ul) == 0ul)
		return 2;

	for(i = 3ul; (i*i) <= n; i += 2ul) {
		if((n % i) == 0ul)
			return i;
	}

	return 0ul;
}

/**
 * Check if a number is prime.
 *   @n: The number.
 *   &returns: True if prime.
 */
bool m_isprime64(uint64_t n)
{
	return m_getdiv64(n) == 0ul;
}

/**
 * Compute the smallest divisor of a number.
 *   @n: The number
 *   &returns: The divisor, or zero if prime.
 */
unsigned int m_getdiv(unsigned int n)
{
	unsigned int i;

	if((n & 1) == 0)
		return 2;

	for(i = 3; (i*i) <= n; i += 2) {
		if((n % i) == 0)
			return i;
	}

	return 0;
}

/**
 * Check if a number is prime.
 *   @n: The number.
 *   &returns: True if prime.
 */
bool m_isprime(unsigned int n)
{
	return m_getdiv(n) == 0;
}


/**
 * Compute a 32-bit marc hash.
 *   @left: The left input.
 *   @right: The right input.
 *   &returns: The m-ashed value.
 */
uint32_t mash32(uint32_t a, uint32_t b)
{
	return (m_rot32(a + b, 27) ^ m_rot32(a - b, 19) ^ ((a * 306895657u) + 2070650213u) ^ ((b * 799637117u) + 561162467u)) * 2765093467u;
}

/**
 * Compute a 64-bit marc hash.
 *   @left: The left input.
 *   @right: The right input.
 *   &returns: The m-ashed value.
 */
uint64_t mash64(uint64_t a, uint64_t b)
{
	return (m_rot64(a + b, 39) ^ m_rot64(a - b, 23) ^ ((a * 5451474691133734519ul) + 14642105499382583659ul) ^ ((b * 12494398421538346499ul) + 11411494365777922193ul)) * 14028973624453214561ul;
}

void mash64buf(uint64_t *hash, void *buf, unsigned int nbytes)
{
	while(nbytes >= 8) {
		*hash = mash64(*hash, *(uint64_t *)buf);
		buf += 8;
		nbytes -= 8;
	}

	if(nbytes > 0) {
		uint64_t v = 0;

		memcpy(&v, buf, nbytes);
		*hash = mash64(*hash, v);
	}
}
