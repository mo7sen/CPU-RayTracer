#pragma once

#include "Ray.h"
#include "Color.h"
#include "HitData.h"


class Material
{
public:
	virtual bool scatter(const Ray& ray_in, const HitData& hit_data, Color& attenutation, Ray& ray_scattered) const = 0;
};
