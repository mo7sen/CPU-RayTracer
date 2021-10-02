#pragma once
#include <Traceable.h>
#include <Texture.h>
#include <Material.h>
#include <materials/IsotropicMaterial.h>

class VolumeTraceable : public Traceable
{
public:
	std::shared_ptr<Material> phase_function;
	std::shared_ptr<Traceable> boundary;
	double neg_inv_density;

public:
	VolumeTraceable(std::shared_ptr<Traceable> b, double d, std::shared_ptr<Texture> t)
		: boundary(b),
		  neg_inv_density(- 1 / d),
			phase_function(std::make_shared<IsotropicMaterial>(t))
	{}

	VolumeTraceable(std::shared_ptr<Traceable> b, double d, Color c)
		: boundary(b),
		  neg_inv_density(- 1 / d),
			phase_function(std::make_shared<IsotropicMaterial>(c))
	{}


	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		return boundary->bounding_box(time0, time1, aabb);
	}
};

