#include <Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <extern/stb/stb_image.h>

Color Image::getUV(float u, float v) const
{
  if(data == nullptr)
  {
    return Color(0.0, 1.0, 0.0);
  }

  u = clamp_d(u, 0.0, 1.0);
  v = 1.0 - clamp_d(v, 0.0, 1.0);

  auto i = static_cast<int32_t>(u * width);
  auto j = static_cast<int32_t>(v * height);

  if (i >= width)
  {
    i = width - 1;
  }

  if (j >= height)
  {
    j = height - 1;
  }

  return getTexel(i, j);
}

Color Image::getTexel(int32_t i, int32_t j) const
{
  const auto color_scale = 1.0 / 255.0;
  auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
	return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}

Image Image::fromFile(std::string image_path)
{
  Image im;
  auto components_per_pixel = bytes_per_pixel;
  im.data = stbi_load(image_path.c_str(), &im.width, &im.height, &components_per_pixel, components_per_pixel);

  if(!im.data)
  {
    std::cerr << "Error: Could not load image file: \"" << image_path << "\".\n";
    exit(1);
    im.width = im.height = -1;
  }

  im.bytes_per_scanline = bytes_per_pixel * im.width;

  return im;
}

Image::Image(std::string image_path)
{
  auto components_per_pixel = bytes_per_pixel;
  data = stbi_load(image_path.c_str(), &width, &height, &components_per_pixel, components_per_pixel);

  if(!data)
  {
    std::cerr << "Error: Could not load image file: \"" << image_path << "\".\n";
    exit(1);
    width = height = -1;
  }

  bytes_per_scanline = bytes_per_pixel * width;
}

Image::~Image()
{
  stbi_image_free(data);
}
