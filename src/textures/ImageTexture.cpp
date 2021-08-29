#include <textures/ImageTexture.h>
#include <Image.h>

ImageTexture::ImageTexture(std::string image_path) 
{
	image = std::make_shared<Image>(image_path);
}


Color ImageTexture::value(double u, double v, const Point3& p) const
{
	return image->getUV(u, v);
}
