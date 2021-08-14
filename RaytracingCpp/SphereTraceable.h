#pragma once
#include "Traceable.h"
#include "Material.h"

class SphereTraceable : public Traceable
{
public:
	Point3 center;
	real radius;
	std::shared_ptr<Material> material;

public:
	SphereTraceable() = default;
	SphereTraceable(Point3 c, real r, std::shared_ptr<Material> m) : center(c), radius(r), material(m) {}

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	
};

