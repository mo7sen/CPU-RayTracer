#pragma once

#include "Color.h"
#include "Point3.h"

class Texture 
{
public:
	Color value(double u, double v, const Point3& p) const { return Color(0.0, 0.0, 0.0); };
};
