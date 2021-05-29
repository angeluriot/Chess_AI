#include "utils.h"

uint16_t screen_width;

// Donne un réel entre min et max

float random_float(float min, float max)
{
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

// Donne un entier entre min et max

uint32_t random_int(int32_t min, int32_t max)
{
	return rand() % (max - min) + min;
}

// Donne "vrai" avec une probabilité choisie

bool rand_probability(const double& probability)
{
	if (probability < 0.)
		return false;

	if (probability < 1.)
		return (rand() / static_cast<double>(RAND_MAX) < probability);

	return true;
}

uint32_t random_32()
{
	static uint32_t state = 1804289383;
	return (state = state ^ (state << 13) ^ (state >> 17) ^ (state << 5));
}

uint64_t random_64()
{
	uint64_t randoms[4] = { random_32() & 0xFFFF, random_32() & 0xFFFF, random_32() & 0xFFFF, random_32() & 0xFFFF };
	return randoms[0] | (randoms[1] << 16) | (randoms[2] << 32) | (randoms[3] << 48);
}
