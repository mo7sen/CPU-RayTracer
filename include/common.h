#ifndef RT_COMMON_HEADER
#define RT_COMMON_HEADER

// Headers
#include <stdint.h>
#include <iostream>
#include <cmath>
#include <memory>

#ifdef _WIN32
#define __rtinline __forceinline
#else
#define __rtinline inline
#endif

// Alias
using real = float;

// Constants
const auto INF = std::numeric_limits<real>::infinity();
const double PI = double(3.1415926535897932385);

// Utilities
__rtinline real deg2rad(real deg) { return deg * PI / real(180); }
__rtinline real rad2deg(real rad) { return rad * real(180) / PI; }

static uint64_t mcg_state = 0xcafef00dd15ea5e5u;	// Must be odd
static uint64_t const multiplier = 6364136223846793005u;

__rtinline uint32_t pcg32_fast(void)
{
	uint64_t x = mcg_state;
	unsigned count = (unsigned)(x >> 61);	// 61 = 64 - 3

	mcg_state = x * multiplier;
	x ^= x >> 22;
	return (uint32_t)(x >> (22 + count));	// 22 = 32 - 3 - 7
}

static unsigned int g_seed = 0xFABAEABF;

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
__rtinline float random_float(void) {
    g_seed = (214013*g_seed+2531011);
    return float((g_seed>>16)&0x7FFF) / float(32768);
}

#include <cstdlib>
// __rtinline float random_float() { return (float(rand()) / (float(RAND_MAX) + 1.0f)); }
// __rtinline float random_float() { return (float(pcg32_fast()) / (float(UINT_MAX) + 1.0)); }
__rtinline float random_float(float min, float max) { return min + ((max - min) * random_float()); }

__rtinline int random_int(int min, int max) { return static_cast<int>(random_float(min, max+1)); }

__rtinline float clamp_f(float x, float min, float max) { return (x < min) ? min : (x > max) ? max : x; }
__rtinline double clamp_d(double x, double min, double max) { return (x < min) ? min : (x > max) ? max : x; }

#include <Vec3.h>

#endif
