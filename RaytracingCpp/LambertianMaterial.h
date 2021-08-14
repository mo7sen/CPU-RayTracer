#pragma once

#include "Color.h"
#include "Material.h"

class LambertianMaterial : public Material
{
public:
	LambertianMaterial(const Color& a) : albedo(a) {}

	virtual bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const override
	{
		auto scatterDirection = hitData.hitNormal + unit_vector((random_vec_in_unit_sphere()));

		if (scatterDirection.near_zero()) // Edge case where random vector = -hitNormal
			scatterDirection = hitData.hitNormal;

		ray_scattered = Ray(hitData.hitPos, unit_vector(scatterDirection));
		attenuation = albedo;
		return true;
	}

public:
	Color albedo;
};
