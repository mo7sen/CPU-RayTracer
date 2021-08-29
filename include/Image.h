#pragma once
#include <common.h>
#include <Color.h>

class Image
{
public:
  const static int32_t bytes_per_pixel = 3;
private:
  unsigned char *data;
  int32_t width;
  int32_t height;
  int32_t bytes_per_scanline;

public:
  Image() = default;
  Image(std::string image_path);
  ~Image();

  Color getUV(float u, float v) const;
  Color getTexel(int32_t x, int32_t y) const;

  static Image fromFile(std::string image_path);
};
