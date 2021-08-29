#include <common.h>
#include <Color.h>
#include <MSColor.h>
#include <Camera.h>
#include <omp.h>

#include <Ray.h>
#include <HitData.h>

#include <render_targets/RenderTargetPPMImage.h>

#include <textures/ImageTexture.h>
#include <textures/CheckeredTexture.h>

#include <traceables/SphereTraceable.h>
#include <traceables/ListTraceable.h>
#include <traceables/BVHNode.h>
#include <traceables/MovingSphereTraceable.h>
#include <traceables/StaticMeshTraceable.h>

#include <materials/LambertianMaterial.h>
#include <materials/MetalMaterial.h>
#include <materials/DielectricMaterial.h>
#include <materials/DiffuseLightMaterial.h>

Color rayColor(const Ray& r, const Texture& background, const Traceable& scene, int32_t depth)
{
	if (depth <= 0) return Color(0.0);

	HitData hitData;
	if(!scene.hit(r, 0.001, INF, hitData))
	{
		float u, v;
		SphereTraceable::getUV(normalize(r.direction() * -1.0), u, v);
		return background.value(u, v, Vec3f(0.0));
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

int main(int argc, char** argv)
{
	// Image
	const auto aspect_ratio = real(16.0 / 9.0);
	const int32_t image_width = 1280;
	const int32_t image_height = static_cast<int>(image_width / aspect_ratio);

	auto render_target = RenderTargetPPMImage(image_width, image_height);
	render_target.setOutFile("image.ppm");

	// Camera
	real fov = 60;
	Vec3f look_from(0.0f, 200.0f, 100.0f);
	/* Vec3f look_from(-2.0f, 5.0f, 2.0f); */
	// Vec3f look_from(0.0f, 3.0f, 2.0f);
	Vec3f look_at(0.0f, 200.0f, -1.0f);
	/* Vec3f look_at(0.0f, 0.0f, -1.0f); */
	Vec3f up(0.0f, 1.0f, 0.0f);
	auto dist_to_focus = length(look_from - look_at);
	auto aperture = 0.1;

	real frameTime0 = 0.0;
	real frameTime1 = 1.0;
	Camera camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

	/* Color background = Color(0, 0, 0); */
	// Color background = Color(1, 1, 1);
	ImageTexture background("res/earth.jpg");
	/* SolidColorTexture background(Color(0.1, 0.1, 0.1)); */

	// SceneDielectricMaterial
	ListTraceable list;

	/* auto material_ground = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0), 10.0)); */
	/* auto material_center = std::make_shared<LambertianMaterial>(Color(0.1, 0.2, 0.5)); */
	/* auto material_center = std::make_shared<LambertianMaterial>(std::make_shared<ImageTexture>("res/earth.jpg")); */
	/* auto material_left   = std::make_shared<DielectricMaterial>(1.5); */
	auto material_right  = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 0.0);
	/* auto material_light  = std::make_shared<DiffuseLightMaterial>(Color(40.0, 40.0, 32.0)); */

	/* list.add(std::make_shared<SphereTraceable>(Point3( 0.0, -100.5, -1.0), 100.0, material_ground)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3( 0.0,    0.0, -1.0),   0.5, material_center)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0),   0.5, material_left)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0), -0.45, material_left)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3( 1.0,    0.0, -1.0),   0.5, material_right)); */

	/* list.add(std::make_shared<MovingSphereTraceable>(Point3(1.0, 0.0, 1.0), Point3(-1.0, 0.0, 1.0), 0.0, 0.2, 0.5, material_center)); */

	//list.add(std::make_shared<SphereTraceable>(Point3( -1.0, 3.0, 4.0), 0.4, material_light));
	/* auto material_superlight  = std::make_shared<DiffuseLightMaterial>(Color(4.0, 4.0, 3.0)); */
	//list.add(std::make_shared<SphereTraceable>(Point3( 70.0, 200.0, 50.0), 20, material_superlight));

	list.add(std::make_shared<StaticMeshTraceable>("res/IronMan2_PBR.obj", "res"));
	list.add(std::make_shared<SphereTraceable>(Point3(30.0, 200, 50), 20.0, material_right));

	BVHNode bvh(list, frameTime0, frameTime1);
	print_bvh(&bvh, 0);

	// Render
	const int32_t samples_per_pixel = 10;
	const int32_t max_ray_depth = 10;

	omp_set_num_threads(7);

#pragma omp parallel for schedule(static, 2) // collapse(2)
	for (int j = 0; j < image_height; ++j)
	{
		for (int i = 0; i < image_width; ++i)
		{
			MSColor pixel_color;
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = real(i + random_float()) / (image_width-1);
				auto v = real(j + random_float()) / (image_height-1);

				// auto u = real(i) / real(image_width - 1);
				// auto v = real(j) / real(image_height - 1);

				auto ray = camera.get_ray(u, v);
				pixel_color += rayColor(ray, background, bvh, max_ray_depth);
			}
			pixel_color.reduce();
			render_target.setPixel(i, j, pixel_color);
		}
		printf("Scanline %d\n", j);
	}
	render_target.renderFrame();
	return 0;
}
