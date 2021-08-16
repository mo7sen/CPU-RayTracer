#include "common.h"
#include "RenderTargetPPMImage.h"
#include "Color.h"
#include "MSColor.h"
#include "Camera.h"
#include <omp.h>

#include "Ray.h"
#include "HitData.h"
#include "Traceable.h"
Color rayColor(const Ray& r, const Color& background, const Traceable& scene, int32_t depth)
{
	if (depth <= 0) return Color(0.0);

	HitData hitData;
	if(!scene.hit(r, 0.001, INF, hitData))
	{
		return background;
	}

	Color emitted = hitData.material->emitted(hitData.u, hitData.v, hitData.hitPos);

	Ray ray_scattered;
	Color attenuation;
	if (hitData.material->scatter(r, hitData, attenuation, ray_scattered))
	{
		return emitted + attenuation * rayColor(ray_scattered, background, scene, depth - 1);
	}

	return emitted;
}

#include "ListTraceable.h"
#include "SphereTraceable.h"
#include "MovingSphereTraceable.h"
#include "LambertianMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"
#include "DiffuseLightMaterial.h"

#include "CheckeredTexture.h"
#include "ImageTexture.h"

int main(int argc, char** argv)
{
	// Image
	const auto aspect_ratio = real(16.0 / 9.0);
	const int32_t image_width = 1280;
	const int32_t image_height = static_cast<int>(image_width / aspect_ratio);

	auto render_target = RenderTargetPPMImage(image_width, image_height);
	render_target.setOutFile("image.ppm");

	// Camera
	real fov = 90;
	Vec3 look_from(-3, 3, 2);
	Vec3 look_at(0, 0, -1);
	Vec3 up(0, 1, 0);
	auto dist_to_focus = (look_from - look_at).length();
	auto aperture = 0.1;

	Camera camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, 0, 1);

	Color background = Color(0.0, 0.0, 0.0);

	// SceneDielectricMaterial
	ListTraceable list;

	auto material_ground = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0), 10.0));
	// auto material_center = std::make_shared<LambertianMaterial>(Color(0.1, 0.2, 0.5));
	auto material_center = std::make_shared<LambertianMaterial>(std::make_shared<ImageTexture>("res/earth.jpg"));
	auto material_left   = std::make_shared<DielectricMaterial>(1.5);
	auto material_right  = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 0.0);
	auto material_light  = std::make_shared<DiffuseLightMaterial>(Color(1.0, 1.0, 1.0));

	list.add(std::make_shared<SphereTraceable>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground));
	list.add(std::make_shared<SphereTraceable>(Point3( 0.0,    0.0, -1.0),   0.5, material_center));
	list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0),   0.5, material_left));
	list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0), -0.45, material_left));
	list.add(std::make_shared<SphereTraceable>(Point3( 1.0,    0.0, -1.0),   0.5, material_right));

	list.add(std::make_shared<MovingSphereTraceable>(Point3( 1.0, 0.0, 1.0), Point3( -1.0, 0.0, 1.0), 0.0, 0.2, 0.5, material_center));

	list.add(std::make_shared<SphereTraceable>(Point3( 0.0, 2.0, 0.0), 0.4, material_light));

	// Render
	const int32_t samples_per_pixel = 1000;
	const int32_t max_ray_depth = 30;

    omp_set_num_threads(10);
	const uint32_t tile_size_x = 16;
	const uint32_t tile_size_y = 16;

	const uint32_t tile_count_x = (image_width + tile_size_x - 1) / tile_size_x;
	const uint32_t tile_count_y = (image_height + tile_size_y - 1) / tile_size_y;

#pragma omp parallel for collapse(2)
	for(int tile_idx_x = 0; tile_idx_x < tile_count_x; tile_idx_x++) 
	{
		for(int tile_idx_y = 0; tile_idx_y < tile_count_y; tile_idx_y++)
		{
			// Render Tile
			for(int pixel_in_tile_x = tile_idx_x * tile_size_x + 0; 
				pixel_in_tile_x < ((tile_idx_x + 1) * tile_size_x) && pixel_in_tile_x < render_target.getWidth();
				pixel_in_tile_x++)
			{
				for(int pixel_in_tile_y = tile_idx_y * tile_size_y + 0;
					pixel_in_tile_y < ((tile_idx_y + 1) * tile_size_y) && pixel_in_tile_y < render_target.getHeight();
					pixel_in_tile_y++)
				{
					MSColor pixel_color;
					for (int s = 0; s < samples_per_pixel; ++s)
					{
						auto u = real(pixel_in_tile_x + random_double()) / (image_width-1);
						auto v = real(pixel_in_tile_y + random_double()) / (image_height-1);
						auto ray = camera.get_ray(u, v);
						pixel_color += rayColor(ray, background, list, max_ray_depth);
					}
					pixel_color.reduce();
					render_target.setPixel(pixel_in_tile_x, pixel_in_tile_y, pixel_color);
				}
			}
		}
	}

//#pragma omp parallel for
//	for (int j = render_target.getHeight() - 1; j >= 0; --j)
//	{
//		for (int i = 0; i < render_target.getWidth(); ++i)
//		{
//			MSColor pixel_color;
//			for (int s = 0; s < samples_per_pixel; ++s)
//			{
//				auto u = real(i + random_double()) / (image_width-1);
//				auto v = real(j + random_double()) / (image_height-1);
//				auto ray = camera.get_ray(u, v);
//				pixel_color += rayColor(ray, list, max_ray_depth);
//			}
//			pixel_color.reduce();
//			render_target.setPixel(i, j, pixel_color);
//		}
//	}
	render_target.renderFrame();
	return 0;
}