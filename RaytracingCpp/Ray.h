#pragma once

#include "common.h"
#include "Point3.h"

class Ray
{
public:
	Ray() = default;
	Ray(const Point3& origin, const Vec3& direction)
		: orig(origin), dir(direction) 
	{}

	inline Point3 origin() const { return orig; }
	inline Vec3 direction() const { return dir; }

	inline Point3 at(real t) const { return orig + t * dir; }

public:
	Point3 orig;
	Vec3 dir;
};

