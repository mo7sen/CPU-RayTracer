#include "Color.h"

Color Color::fromUnitVector(const Vec3& v)
{
	return Color(
		(v.x() + real(1.0)) * real(0.5),
		(v.y() + real(1.0)) * real(0.5), 
		(v.z() + real(1.0)) * real(0.5));
}
