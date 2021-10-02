#pragma once
#include <Traceable.h>
#include <Material.h>
#include "ListTraceable.h"

class BoxTraceable : public Traceable
{
public:
	std::shared_ptr<Material> material;
	Point3 box_min;
	Point3 box_max;
	ListTraceable sides;

public:
	BoxTraceable() {}
	BoxTraceable(const Point3 &p0, const Point3 &p1, std::shared_ptr<Material> m);

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		aabb = AABB(box_min, box_max);
		return true;
	}
};

