#pragma once

#include "Material.h"
#include "Texture.h"
#include "SolidColorTexture.h"

class DiffuseLightMaterial : public Material
{
public:
	std::shared_ptr<Texture> emit;

public:
	DiffuseLightMaterial(std::shared_ptr<Texture> a) : emit(a) {}
	DiffuseLightMaterial(Color c) : emit(std::make_shared<SolidColorTexture>(c)) {}

	bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& scattered) const
	{
		return false;
	}

	Color emitted(double u, double v, const Point3& p) const
	{
		return emit->value(u, v, p);
	}
};