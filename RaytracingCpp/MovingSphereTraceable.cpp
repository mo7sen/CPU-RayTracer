#include "MovingSphereTraceable.h"

void MovingSphereTraceable::getUV(const Point3& p, float &u, float &v)
{
	auto theta = acos(-p[1]);
	auto phi = atan2(-p[2], p[0]) + PI;

	u = phi / (2 * PI);
	v = theta / PI;
}

bool MovingSphereTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3f& O = ray.origin();
	const Vec3f& D = ray.direction();
	double time = clamp_f(ray.time(), time0, time1);
	const Vec3f C = lerp(center0, center1, time);

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
	Vec3f outward_normal = (hitData.hitPos - C) / radius;
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	// Calculating texture coordinates
	MovingSphereTraceable::getUV(outward_normal, hitData.u, hitData.v);

	return true;
}

bool MovingSphereTraceable::bounding_box(real t0, real t1, AABB& aabb) const
{
	t0 = std::max(t0, time0);
	t1 = std::min(t1, time1);

	const Vec3f C0 = lerp(center0, center1, t0);
	const Vec3f C1 = lerp(center0, center1, t1);

	const AABB aabb0 = AABB(C0 - Vec3f(radius), C0 + Vec3f(radius));
	const AABB aabb1 = AABB(C1 - Vec3f(radius), C1 + Vec3f(radius));

	aabb = AABB::surrounding(aabb0, aabb1);

	return true;
}
