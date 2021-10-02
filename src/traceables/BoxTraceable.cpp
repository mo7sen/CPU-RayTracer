#include <traceables/BoxTraceable.h>
#include <traceables/AARectTraceable.h>

BoxTraceable::BoxTraceable(const Point3 &p0, const Point3 &p1, std::shared_ptr<Material> m)
{
	box_min = p0;
	box_max = p1;

	sides.add(std::make_shared<XYRectTraceable>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), m));
	sides.add(std::make_shared<XYRectTraceable>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), m));
	sides.add(std::make_shared<XZRectTraceable>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), m));
	sides.add(std::make_shared<XZRectTraceable>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), m));
	sides.add(std::make_shared<YZRectTraceable>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), m));
	sides.add(std::make_shared<YZRectTraceable>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), m));
}

bool BoxTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	return sides.hit(ray, t_min, t_max, hitData);
}
