#pragma once
#include "Texture.h"

class ImageTexture : public Texture
{
public:
	const static int32_t bytes_per_pixel = 3;

private:
	unsigned char *data;
	int32_t width;
	int32_t height;
	int32_t bytes_per_scanline;


public:
	ImageTexture(): data(nullptr), width(0), height(0), bytes_per_scanline(0) {}
	ImageTexture(const char *image_path);

	Color value(double u, double v, const Point3& p) const;

	~ImageTexture() { delete data; }
private:
};

