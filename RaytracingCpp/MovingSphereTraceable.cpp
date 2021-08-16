#include "MovingSphereTraceable.h"

void MovingSphereTraceable::getUV(const Point3& p, double &u, double &v)
{
	auto theta = acos(-p.y());
	auto phi = atan2(-p.z(), p.x()) + PI;

	u = phi / (2 * PI);
	v = theta / PI;
}

bool MovingSphereTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3& O = ray.origin();
	const Vec3& D = ray.direction();
	double time = clamp_d(ray.time(), time0, time1);
	const Vec3 C = center1 * time + center0 * (1.0 - time);

	auto C2O = O - C;

	auto a = D.length_squared();
	auto half_b = dot(D, C2O);
	auto c = C2O.length_squared() - (radius * radius);

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return false;
	auto root = (-half_b - std::sqrt(discriminant)) / a;
	if (root < t_min || root > t_max)
	{   // Try the other sign
		root = (-half_b + std::sqrt(discriminant)) / a;
		if (root < t_min || root > t_max)
			return false;
	}

	hitData.t = root;
	hitData.hitPos = ray.at(hitData.t);
	Vec3 outward_normal = (hitData.hitPos - C) / radius;
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	// Calculating texture coordinates
	MovingSphereTraceable::getUV(outward_normal, hitData.u, hitData.v);

	return true;
}