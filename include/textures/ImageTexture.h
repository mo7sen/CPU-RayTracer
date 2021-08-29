#pragma once
#include <Texture.h>
#include <Image.h>

class ImageTexture : public Texture
{
private:
	std::shared_ptr<Image> image;

public:
	ImageTexture() = default;
	ImageTexture(std::string image_path);
	ImageTexture(std::shared_ptr<Image> src_image): image(src_image) {}

	virtual Color value(double u, double v, const Point3& p) const override;
};

