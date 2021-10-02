#include <traceables/RotateYTraceable.h>

RotateYTraceable::RotateYTraceable(std::shared_ptr<Traceable> _tr, real _theta)
{
	tr = _tr;
	auto radians = deg2rad(_theta);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = _tr->bounding_box(0.0, 1.0, aabb);
	Point3 min(INF);
	Point3 max(-INF);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i*aabb.vMax().x() + (1-i)*aabb.vMin().x();
				auto y = j*aabb.vMax().y() + (1-j)*aabb.vMin().y();
				auto z = k*aabb.vMax().z() + (1-k)*aabb.vMin().z();

				auto newx =  cos_theta*x + sin_theta*z;
				auto newz = -sin_theta*x + cos_theta*z;

				Vec3f tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
						min[c] = fmin(min[c], tester[c]);
						max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	aabb = AABB(min, max);
}

bool RotateYTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	auto O = ray.origin();
	auto D = ray.direction();

	O[0] = cos_theta * ray.origin()[0] - sin_theta * ray.origin()[2];
	O[2] = sin_theta * ray.origin()[0] + cos_theta * ray.origin()[2];

	D[0] = cos_theta * ray.direction()[0] - sin_theta * ray.direction()[2];
	D[2] = sin_theta * ray.direction()[0] + cos_theta * ray.direction()[2];

	Ray r2(O, D, ray.time());

	if(!tr->hit(r2, t_min, t_max, hitData)) {
		return false;
	}

	auto pos = hitData.hitPos;
	auto normal = hitData.hitNormal;

	pos[0] = cos_theta * hitData.hitPos[0] + sin_theta * hitData.hitPos[2];
	pos[2] = -sin_theta * hitData.hitPos[0] + cos_theta * hitData.hitPos[2];

	normal[0] = cos_theta * hitData.hitNormal[0] + sin_theta * hitData.hitNormal[2];
	normal[2] = -sin_theta * hitData.hitNormal[0] + cos_theta * hitData.hitNormal[2];

	hitData.hitPos = pos;
	hitData.set_face_normal(r2, normal);

	return true;
}
