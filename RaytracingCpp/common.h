#ifndef RT_COMMON_HEADER
#define RT_COMMON_HEADER

// Headers
#include <stdint.h>
#include <iostream>
#include <cmath>

// Alias
using real = double;

// Constants
const auto INF = std::numeric_limits<real>::infinity();
const real PI = real(3.1415926535897932385);

// Utilities
inline real deg2rad(real deg) { return deg * PI / real(180); }
inline real rad2deg(real rad) { return rad * real(180) / PI; }

#include <cstdlib>
inline double random_double() { return rand() / (RAND_MAX + 1.0); }
inline double random_double(double min, double max) { return min + ((max - min) * random_double()); }

inline real clamp(real x, real min, real max) { return (x < min) ? min : (x > max) ? max : x; }

#endif
