#pragma once

#include "Ray.h"
#include "Color.h"
#include "HitData.h"


class Material
{
public:
	bool scatter(const Ray& ray_in, const HitData& hit_data, Color& attenutation, Ray& ray_scattered) const { return false; }
	Color emitted(double u, double v, const Point3& p) const
	{
		return Color(0.0, 0.0, 0.0);
	}
};
