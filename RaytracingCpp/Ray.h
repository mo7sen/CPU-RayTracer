#pragma once

#include "common.h"
#include "Point3.h"

class Ray
{
public:
	Ray() = default;
	Ray(const Point3& origin, const Vec3& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	inline Point3 origin() const { return orig; }
	inline Vec3 direction() const { return dir; }
	inline double time() const { return tm; }

	inline Point3 at(real t) const { return orig + t * dir; }

public:
	Point3 orig;
	Vec3 dir;
	double tm;
};

