#pragma once
#include <Traceable.h>
#include <Material.h>

class XYRectTraceable : public Traceable
{
public:
	real x0, x1, y0, y1, k;
	std::shared_ptr<Material> material;

public:
	XYRectTraceable() {}
	XYRectTraceable(real _x0, real _x1, real _y0, real _y1, real _k, std::shared_ptr<Material> m) 
	: x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), material(m) {}

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		aabb = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
		return true;
	}
};

class XZRectTraceable : public Traceable
{
public:
	real x0, x1, z0, z1, k;
	std::shared_ptr<Material> material;

public:
	XZRectTraceable() {}
	XZRectTraceable(real _x0, real _x1, real _z0, real _z1, real _k, std::shared_ptr<Material> m) 
	: x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), material(m) {}

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		aabb = AABB(Point3(x0, k - 0.0001, z0), Point3(x1, k + 0.0001, z1));
		return true;
	}
};

class YZRectTraceable : public Traceable
{
public:
	real y0, y1, z0, z1, k;
	std::shared_ptr<Material> material;

public:
	YZRectTraceable() {}
	YZRectTraceable(real _y0, real _y1, real _z0, real _z1, real _k, std::shared_ptr<Material> m) 
	: y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), material(m) {}

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override {
		aabb = AABB(Point3(k - 0.0001, y0, z0), Point3(k + 0.0001, y1, z1));
		return true;
	}
};
