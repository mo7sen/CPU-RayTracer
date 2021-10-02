#pragma once
#include <Traceable.h>

class RotateYTraceable : public Traceable
{
public:
	RotateYTraceable(const std::shared_ptr<Traceable> _tr, real _theta);

	virtual bool hit(const Ray& r, real t_min, real t_max, HitData& hitData) const override;

	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		aabb = this->aabb;
		return hasbox;
	}

public:
	std::shared_ptr<Traceable> tr;
	real sin_theta;
	real cos_theta;

	bool hasbox;
	AABB aabb;
};

