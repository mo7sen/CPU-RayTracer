#pragma once

#include <Point3.h>
#include <Ray.h>

class AABB
{
public:
	Point3 v_min;
	Point3 v_max;


private:
public:
	AABB() = default;
	AABB(Point3 a, Point3 b): v_min(a), v_max(b) {}

	Point3 vMin() const { return v_min; }
	Point3 vMax() const { return v_max; }

	bool hit(const Ray& ray, real t_min, real t_max) const;

	static AABB surrounding(const AABB& aabb0, const AABB& aabb1);
};

