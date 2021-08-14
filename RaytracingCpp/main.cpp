#include <iostream>
#include <fstream>
#include <vector>
#include "common.h"
#include "Color.h"
#include "MSColor.h"
#include "Camera.h"
#include <omp.h>

const int32_t DEFAULT_WIDTH = 800;
const int32_t DEFAULT_HEIGHT = 800;

class PPMImageRenderFrame {
private:
	int32_t m_width;
	int32_t m_height;

	std::vector<Color> m_pixels;

	void frameStream(std::ostream & os, std::ostream & progstream)
	{
		os << "P3\n" << m_width << ' ' << m_height << "\n255\n";

		for (int j = m_height - 1; j >= 0; --j)
		{
			progstream << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < m_width; ++i)
			{
				int32_t idx = j * m_width + i;
				Color pixelColor = m_pixels[idx];
				os << pixelColor << ' ';
			}
			os << std::endl;
		}
		progstream << "\nDone.\n";
	}

public:
	PPMImageRenderFrame() : PPMImageRenderFrame(DEFAULT_WIDTH, DEFAULT_HEIGHT) {};
	PPMImageRenderFrame(int32_t width, int32_t height)
		: m_width(width), m_height(height) 
	{
		std::vector<Color> pixels(size_t(m_width) * size_t(m_height));
		m_pixels = pixels;
	}

	void setHeight(int32_t height)
	{
		m_height = height;
	}

	void setWidth(int32_t width)
	{
		m_width = width;
	}

	int32_t getHeight() const
	{
		return m_height;
	}

	int32_t getWidth() const
	{
		return m_width;
	}

	void renderFrameToConsole()
	{
		frameStream(std::cout, std::cerr);
	}

	void renderFrameToFile(std::string filename)
	{
		std::ofstream imageFile;
		imageFile.open (filename);
		frameStream(imageFile, std::cerr);
		imageFile.close();
	}

	void setPixel(int32_t x, int32_t y, Color const& color)
	{
		int32_t idx = y * m_width + x;
		m_pixels[idx] = color;
	}

	Color getPixel(int32_t x, int32_t y) const
	{
		int32_t idx = y * m_width + x;
		return m_pixels[idx];
	}
	
	~PPMImageRenderFrame() = default;
};

#include "Ray.h"
#include "HitData.h"
#include "Traceable.h"
Color rayColor(const Ray& r, const Traceable& scene, int32_t depth)
{
	if (depth <= 0) return Color(0.0);

	HitData hitData;
	if (scene.hit(r, 0.001, INF, hitData))
	{
		Ray ray_scattered;
		Color attenuation;
		if (hitData.material->scatter(r, hitData, attenuation, ray_scattered))
		{
			return attenuation * rayColor(ray_scattered, scene, depth - 1);
		}
		return Color(0, 0, 0);
	}

	Vec3 unit_dir = unit_vector(r.direction());
	auto t = real(0.5) * (unit_dir.y() + real(1.0));
	return (real(1.0) - t) * Color(real(1.0)) + t * Color(real(0.5), real(0.7), real(1.0));
}

#include "ListTraceable.h"
#include "SphereTraceable.h"
#include "LambertianMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"
int main(int argc, char** argv)
{
	// Image
	const auto aspect_ratio = real(16.0 / 9.0);
	const int32_t image_width = 1920;
	const int32_t image_height = static_cast<int>(image_width / aspect_ratio);

	PPMImageRenderFrame image(image_width, image_height);

	// Camera
	real fov = 90;
	Vec3 look_from(3, 3, 2);
	Vec3 look_at(0, 0, -1);
	Vec3 up(0, 1, 0);
	auto dist_to_focus = (look_from - look_at).length();
	auto aperture = 0.5;

	Camera camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus);

	// SceneDielectricMaterial
	ListTraceable list;

	auto material_ground = std::make_shared<LambertianMaterial>(Color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<LambertianMaterial>(Color(0.1, 0.2, 0.5));
	auto material_left   = std::make_shared<DielectricMaterial>(1.5);
	auto material_right  = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 0.0);

	list.add(std::make_shared<SphereTraceable>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground));
	list.add(std::make_shared<SphereTraceable>(Point3( 0.0,    0.0, -1.0),   0.5, material_center));
	list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
	list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0), -0.45, material_left));
	list.add(std::make_shared<SphereTraceable>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));

	// Render
	const int32_t samples_per_pixel = 100;
	const int32_t max_ray_depth = 30;

    omp_set_num_threads(8);

	uint32_t scanlines_remaining = image.getHeight();

#pragma omp parallel for
	for (int j = image.getHeight() - 1; j >= 0; --j)
	{
		for (int i = 0; i < image.getWidth(); ++i)
		{
			MSColor pixel_color;
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = real(i + random_double()) / (image_width-1);
				auto v = real(j + random_double()) / (image_height-1);
				auto ray = camera.get_ray(u, v);
				pixel_color += rayColor(ray, list, max_ray_depth);
			}
			pixel_color.reduce();
			image.setPixel(i, j, pixel_color);
		}
#pragma omp atomic
  	    scanlines_remaining--;
#pragma omp critical 
		{
			std::cerr << "\rScanlines remaining: " << scanlines_remaining << ' ' << std::flush;
		}
	}
	image.renderFrameToFile("image.ppm");
	return 0;
}