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
	SphereTraceable() {}
	SphereTraceable(Point3 c, real r, std::shared_ptr<Material> m) : center(c), radius(r), material(m) {}

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override;

	static void getUV(const Point3& p, float& u, float& v);
	
};

