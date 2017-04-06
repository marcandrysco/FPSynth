#ifndef EXTRA_H
#define EXTRA_H

/**
 * Rotate a 32-bit value.
 *   @v: The value.
 *   @rot: The rotate amount.
 *   &returns: The rotated value.
 */
static inline uint32_t m_rot32(uint32_t v, int8_t rot)
{
	return (v >> rot) | (v << (32 - rot));
}

/**
 * Rotate a 64-bit value.
 *   @v: The value.
 *   @rot: The rotate amount.
 *   &returns: The rotated value.
 */
static inline uint64_t m_rot64(uint64_t v, int8_t dir)
{
	return (v >> dir) | (v << (64 - dir));
}


/*
 * mash declarations
 */
uint32_t mash32(uint32_t a, uint32_t b);
uint64_t mash64(uint64_t a, uint64_t b);
void mash64buf(uint64_t *hash, void *buf, unsigned int nbytes);

#endif
