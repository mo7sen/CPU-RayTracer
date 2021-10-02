#pragma once
#include <Traceable.h>

class TranslateTraceable : public Traceable
{
public:
	TranslateTraceable() {}
	TranslateTraceable(const std::shared_ptr<Traceable> _tr, Vec3f _p)
		: tr(_tr), translation(_p)
	{}

	virtual bool hit(const Ray& r, real t_min, real t_max, HitData& hitData) const override {
		Ray r2(r.origin() - translation, r.direction(), r.time());
		const auto hit = tr->hit(r2, t_min, t_max, hitData);
		if(!hit) {
			return false;
		}

		hitData.hitPos += translation;
		hitData.set_face_normal(r2, hitData.hitNormal);
		return true;
	}

	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		const auto inner_aabb = tr->bounding_box(time0, time1, aabb);
		if(!inner_aabb) {
			return false;
		}

		aabb = AABB(aabb.vMin() + translation, aabb.vMax() + translation);
		return true;
	}

public:
	std::shared_ptr<Traceable> tr;
	Vec3f translation;
};
