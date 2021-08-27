#include "ImageTexture.h"
#include "extern/stb_image.h" 

ImageTexture::ImageTexture(const char *image_path) 
{
	auto components_per_pixel = bytes_per_pixel;

	data = stbi_load(image_path, &width, &height, &components_per_pixel, components_per_pixel);

	if(!data) 
	{
		std::cerr << "Error: Could not load texture image file '" << image_path << "'.\n";
		exit(1);
		width = height = 0;
	}
	bytes_per_scanline = bytes_per_pixel * width;
}

Color ImageTexture::value(double u, double v, const Point3& p) const
{
	if(data == nullptr)
	{
		return Color(0.0, 1.0, 1.0);
	}
	u = clamp_d(u, 0.0, 1.0);
	v = 1.0 - clamp_d(v, 0.0, 1.0);

	auto i = static_cast<int>(u * width);
	auto j = static_cast<int>(v * height);

	if(i >= width)
	{
		i = width - 1;
	}

	if(j >= height)
	{
		j = height - 1;
	}

	const auto color_scale = 1.0 / 255.0;
	auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

	return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}