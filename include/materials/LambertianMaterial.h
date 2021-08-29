#pragma once

#include <Color.h>
#include <Material.h>
#include <Texture.h>
#include <textures/SolidColorTexture.h>

class LambertianMaterial : public Material
{
public:
	LambertianMaterial(const Color& a) : albedo(std::make_shared<SolidColorTexture>(a)) {}
	LambertianMaterial(std::shared_ptr<Texture> a) : albedo(a) {}

	bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
	{
		auto scatterDirection = hitData.hitNormal + normalize((random_vec_in_unit_sphere()));

		if (near_zero(scatterDirection)) // Edge case where random vector = -hitNormal
			scatterDirection = hitData.hitNormal;

		ray_scattered = Ray(hitData.hitPos, normalize(scatterDirection), ray_in.time());
		attenuation = albedo->value(hitData.u, hitData.v, hitData.hitPos);
		return true;
	}

public:
	std::shared_ptr<Texture> albedo;
};
