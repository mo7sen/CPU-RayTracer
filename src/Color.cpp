#include "Color.h"

Color Color::fromUnitVector(const Vec3f& v)
{
	Vec3f mapped = (v + Vec3f(1.0, 1.0, 1.0)) / 2.0;
	return Color(mapped.x(), mapped.y(), mapped.z());
}
