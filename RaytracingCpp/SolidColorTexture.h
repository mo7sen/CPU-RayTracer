#pragma once

#include "Texture.h"

class SolidColorTexture : public Texture
{
private:
	Color color_value;
public:
	SolidColorTexture() {}
	SolidColorTexture(Color c): color_value(c) {}

	SolidColorTexture(double red, double green, double blue):
		SolidColorTexture(Color(red, green, blue)) {}

	virtual Color value(double u, double v, const Point3& p) const override {
		return color_value;
	}
};

