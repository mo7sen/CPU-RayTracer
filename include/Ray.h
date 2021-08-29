#pragma once

#include <common.h>
#include <Point3.h>

class Ray
{
public:
	Ray() = default;
	__rtinline explicit Ray(const Point3& origin, const Vec3f& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	__rtinline explicit Ray(Point3&& origin, Vec3f&& direction, double time = 0.0)
		: orig(origin), dir(direction), tm(time)
	{}

	__rtinline Point3 origin() const { return orig; }
	__rtinline Vec3f direction() const { return dir; }
	__rtinline double time() const { return tm; }

	__rtinline Point3 at(real t) const { return orig + t * dir; }

public:
	Point3 orig;
	Vec3f dir;
	double tm;
};

