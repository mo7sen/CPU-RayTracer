#pragma once

#include "common.h"
#include "Point3.h"

class Ray
{
public:
	Ray() = default;
	__forceinline explicit Ray(const Point3& origin, const Vec3f& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	__forceinline explicit Ray(Point3&& origin, Vec3f&& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	__forceinline Point3 origin() const { return orig; }
	__forceinline Vec3f direction() const { return dir; }
	__forceinline double time() const { return tm; }

	__forceinline Point3 at(real t) const { return orig + t * dir; }

public:
	Point3 orig;
	Vec3f dir;
	double tm;
};

