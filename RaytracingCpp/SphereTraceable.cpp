#include "SphereTraceable.h"

void SphereTraceable::getUV(const Point3& p, float &u, float &v)
{
	auto theta = acos(-p.y());
	auto phi = atan2(-p.z(), p.x()) + PI;

	u = phi / (2 * PI);
	v = theta / PI;
}

bool SphereTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3f& O = ray.orig;
	const Vec3f& D = ray.dir;
	const Vec3f& C = center;

	auto C2O = O - C;

	auto a = lengthSq(D);
	auto half_b = dot(D, C2O);
	auto c = lengthSq(C2O) - (radius * radius);

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
	Vec3f outward_normal = (hitData.hitPos - center) / radius;
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	// Calculating texture coordinates
	SphereTraceable::getUV(outward_normal, hitData.u, hitData.v);

	return true;
}

bool SphereTraceable::bounding_box(real time0, real time1, AABB& aabb) const
{
	auto r = abs(Vec3f(radius));
	aabb = AABB(center - r, center + r);
	return true;
}
