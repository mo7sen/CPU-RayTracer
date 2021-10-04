#pragma once
#include <Point3.h>

class Material; 

class HitData
{
public:
	Point3 hitPos;
	Vec3f hitNormal;
	Vec3f hitTangent;
	Vec3f hitBitangent;

	real t;
	std::shared_ptr<Material> material;
	bool front; // Ray entering the object

	float u;
	float v;

public:
	HitData() = default;

	inline void set_face_normal(const Ray& ray, const Vec3f& outward_normal)
	{
		front = dot(ray.direction(), outward_normal) < 0;
		hitNormal = front ? outward_normal : -outward_normal;
	}
};
