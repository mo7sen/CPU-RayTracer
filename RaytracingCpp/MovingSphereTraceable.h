#pragma once

#include "Traceable.h"
#include "Material.h"

class MovingSphereTraceable : public Traceable
{
public:
	Point3 center0;
	Point3 center1;
	double time0;
	double time1;
	real radius;
	std::shared_ptr<Material> material;

public:
	MovingSphereTraceable() = default;
	MovingSphereTraceable(Point3 c0, Point3 c1, double _time0, double _time1, real r, std::shared_ptr<Material> m) : center0(c0), center1(c1), time0(_time0), time1(_time1), radius(r), material(m) {}

	bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const;
private:
	static void getUV(const Point3& p, double& u, double& v);
};