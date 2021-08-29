#pragma once

#include <common.h>
#include "../Material.h"

class DielectricMaterial : public Material
{
public:
	DielectricMaterial(real index_of_refraction) : ir(index_of_refraction) {}

	bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
	{
		attenuation = Color(1.0);
		real refraction_ratio = hitData.front ? (1.0 / ir) : ir;
		Vec3f unit_direction = normalize(ray_in.direction());

		double cos_theta = fmin(dot(-unit_direction, hitData.hitNormal), 1.0);
		double sin_theta = sqrt(1 - cos_theta * cos_theta);

		bool total_internal_reflection = (refraction_ratio * sin_theta) > 1.0;

		Vec3f scatter_direction;

		if (total_internal_reflection || reflectance(cos_theta, refraction_ratio) > random_float())
			scatter_direction = reflect(unit_direction, hitData.hitNormal);
		else
			scatter_direction = refract(unit_direction, hitData.hitNormal, refraction_ratio);

		ray_scattered = Ray(hitData.hitPos, scatter_direction, ray_in.time());
		return true;
	}

private:
	inline static real reflectance(real cos_theta, real ref_idx)
	{
		auto r0 = (1 - ref_idx) / (1 + ref_idx); // this is sqrt(r0)
		r0 = r0 * r0;
		return r0 + ((1 - r0) * pow((1 - cos_theta), 5));
	}

public:
	real ir;
};
