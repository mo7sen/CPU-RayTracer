#pragma once

#include <Color.h>
#include <Material.h>
#include <Texture.h>
#include <textures/SolidColorTexture.h>

class IsotropicMaterial : public Material
{
public:
	IsotropicMaterial(const Color& a) : albedo(std::make_shared<SolidColorTexture>(a)) {}
	IsotropicMaterial(std::shared_ptr<Texture> a) : albedo(a) {}

	bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
	{
		ray_scattered = Ray(hitData.hitPos, random_vec_in_unit_sphere(), ray_in.time());
		attenuation = albedo->value(hitData.u, hitData.v, hitData.hitPos);
		return true;
	}

public:
	std::shared_ptr<Texture> albedo;
};
