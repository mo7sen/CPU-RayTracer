#pragma once

#include <Texture.h>
#include <textures/SolidColorTexture.h>

class CheckeredTexture : public Texture
{
	// Members
private:
	std::shared_ptr<Texture> odd;
	std::shared_ptr<Texture> even;

	uint32_t checker_rate;
public:

	// Methods
private:
public:
	CheckeredTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd, uint32_t _checker_rate): even(_even), odd(_odd), checker_rate(_checker_rate) {}
	CheckeredTexture(Color c1, Color c2, uint32_t _checker_rate): even(std::make_shared<SolidColorTexture>(c1)), odd(std::make_shared<SolidColorTexture>(c2)), checker_rate(_checker_rate) {}

	Color value(double u, double v, const Point3 &p) const;
};

