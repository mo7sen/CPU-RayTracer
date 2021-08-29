#include <textures/CheckeredTexture.h>

Color CheckeredTexture::value(double u, double v, const Point3& p) const
{
	auto sines = sin(checker_rate * p.x()) * sin(checker_rate * p.y()) * sin(checker_rate * p.z());
	if (sines > 0) 
	{
		return odd->value(u, v, p);
	}
	else
	{
		return even->value(u, v, p);
	}
}
