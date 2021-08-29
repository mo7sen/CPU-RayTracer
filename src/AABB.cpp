#include "AABB.h"

bool AABB::hit(const Ray& ray, real t_min, real t_max) const
{
	auto t0_0 = (v_min - ray.origin()) / ray.direction();
	auto t1_0 = (v_max - ray.origin()) / ray.direction();

	auto t0 = min(t0_0, t1_0);
	auto t1 = max(t0_0, t1_0);

	t_min = std::max(hmax(t0), t_min);
	t_max = std::min(hmin(t1), t_max);

	if(t_max < t_min) {
		return false;
	}

	return true;
}

AABB AABB::surrounding(const AABB& aabb0, const AABB& aabb1)
{
	auto vmin = min(aabb0.v_min, aabb1.v_min);
	auto vmax = max(aabb0.v_max, aabb1.v_max);
	return AABB(vmin, vmax);
}

