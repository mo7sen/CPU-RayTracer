#include "common.h"
#include "RenderTargetPPMImage.h"
#include "Color.h"
#include "MSColor.h"
#include "Camera.h"
#include <omp.h>

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
	const int32_t image_width = 640;
	const int32_t image_height = static_cast<int>(image_width / aspect_ratio);

	auto render_target = RenderTargetPPMImage(image_width, image_height);
	render_target.setOutFile("image.ppm");

	// Camera
	real fov = 60;
	Vec3 look_from(-6, 3, 2);
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

    omp_set_num_threads(10);

	uint32_t scanlines_remaining = render_target.getHeight();

#pragma omp parallel for
	for (int j = render_target.getHeight() - 1; j >= 0; --j)
	{
		for (int i = 0; i < render_target.getWidth(); ++i)
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
			render_target.setPixel(i, j, pixel_color);
		}
#pragma omp atomic
  	    scanlines_remaining--;
#pragma omp critical 
		{
			std::cerr << "\rScanlines remaining: " << scanlines_remaining << ' ' << std::flush;
		}
	}
	render_target.renderFrame();
	return 0;
}