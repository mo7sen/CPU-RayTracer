#pragma once
#include "Point3.h"

class Material; 

class HitData
{
public:
	Point3 hitPos;
	Vec3 hitNormal;
	real t;
	std::shared_ptr<Material> material;
	bool front; // Ray entering the object

	double u;
	double v;

public:
	HitData() = default;

	inline void set_face_normal(const Ray& ray, const Vec3& outward_normal)
	{
		front = dot(ray.direction(), outward_normal) < 0;
		hitNormal = front ? outward_normal : -outward_normal;
	}
};
