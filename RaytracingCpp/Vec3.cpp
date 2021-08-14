#include "Vec3.h"

real Vec3::length() const
{
	return real(std::sqrt(double(length_squared())));
}

real Vec3::length_squared() const
{
	return data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
}

bool Vec3::near_zero() const
{
	const auto s = 1e-8;
	if (
		   abs(data[0]) < s 
		&& abs(data[1]) < s 
		&& abs(data[2]) < s)
		return true;
	return false;
}
