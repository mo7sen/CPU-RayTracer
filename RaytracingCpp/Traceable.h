#pragma once

#include "Point3.h"
#include "Ray.h"
#include "Material.h"
#include "HitData.h"

class Traceable
{
public:
	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const = 0;
};
