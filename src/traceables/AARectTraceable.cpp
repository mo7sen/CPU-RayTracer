#include <traceables/AARectTraceable.h>

bool XYRectTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3f& O = ray.orig;
	const Vec3f& D = ray.dir;

	const auto t = (k - O.z()) / D.z();
	if(t < t_min || t > t_max) {
		return false;
	}

	const auto x = O.x() + D.x() * t;
	const auto y = O.y() + D.y() * t;

	if(x < x0 || x > x1 || y < y0 || y > y1) {
		return false;
	}

	hitData.u = (x - x0) / (x1 - x0);
	hitData.v = (y - y0) / (y1 - y0);

	hitData.t = t;

	hitData.hitPos = Vec3f(x, y, k);

	Vec3f outward_normal = Vec3f(0, 0, 1);
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	return true;
}

bool XZRectTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3f& O = ray.orig;
	const Vec3f& D = ray.dir;

	const auto t = (k - O.y()) / D.y();
	if(t < t_min || t > t_max) {
		return false;
	}

	const auto x = O.x() + D.x() * t;
	const auto z = O.z() + D.z() * t;

	if(x < x0 || x > x1 || z < z0 || z > z1) {
		return false;
	}

	hitData.u = (x - x0) / (x1 - x0);
	hitData.v = (z - z0) / (z1 - z0);

	hitData.t = t;

	hitData.hitPos = Vec3f(x, k, z);

	Vec3f outward_normal = Vec3f(0, 1, 0);
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	return true;
}

bool YZRectTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const Vec3f& O = ray.orig;
	const Vec3f& D = ray.dir;

	const auto t = (k - O.x()) / D.x();
	if(t < t_min || t > t_max) {
		return false;
	}

	const auto y = O.y() + D.y() * t;
	const auto z = O.z() + D.z() * t;

	if(y < y0 || y > y1 || z < z0 || z > z1) {
		return false;
	}

	hitData.u = (y - y0) / (y1 - y0);
	hitData.v = (z - z0) / (z1 - z0);

	hitData.t = t;

	hitData.hitPos = Vec3f(k, y, z);

	Vec3f outward_normal = Vec3f(1, 0, 0);
	hitData.set_face_normal(ray, outward_normal); // Corrects the normal direction so that it is always opposite to the direction of the ray
	hitData.material = material;
	
	return true;
}
