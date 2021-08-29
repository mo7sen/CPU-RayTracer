#pragma once

#include <Point3.h>
#include <Ray.h>
#include <Material.h>
#include <HitData.h>
#include <AABB.h>

class Traceable
{
public:
	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const = 0;
	virtual bool bounding_box(real time0, real time1, AABB& output_box) const = 0;
};
