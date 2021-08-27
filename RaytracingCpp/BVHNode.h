#pragma once
#include "Traceable.h"
#include "ListTraceable.h"

class BVHNode : public Traceable
{
public:
	BVHNode() {}
	BVHNode(const ListTraceable& list, real time0, real time1)
		:BVHNode(list.objects, 0, list.objects.size(), time0, time1)
	{}
	BVHNode(
		const std::vector<std::shared_ptr<Traceable>>& src_objects,
		size_t start, size_t end, real time0, real time1);

	virtual bool hit(const Ray& r, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override;

public:
	std::shared_ptr<Traceable> left;
	std::shared_ptr<Traceable> right;
	AABB box;
};

void print_bvh(const Traceable* bvh, int level);

