#include <common.h>
#include <Color.h>
#include <MSColor.h>
#include <Camera.h>
#include <omp.h>

#include <Ray.h>
#include <HitData.h>

#include <render_targets/RenderTargetPPMImage.h>
#include <render_targets/RenderTargetPNGImage.h>

#include <textures/ImageTexture.h>
#include <textures/CheckeredTexture.h>

#include <traceables/SphereTraceable.h>
#include <traceables/ListTraceable.h>
#include <traceables/BVHNode.h>
#include <traceables/MovingSphereTraceable.h>
#include <traceables/StaticMeshTraceable.h>
#include <traceables/GLTFStaticMeshTraceable.h>

#include <materials/LambertianMaterial.h>
#include <materials/MetalMaterial.h>
#include <materials/DielectricMaterial.h>
#include <materials/DiffuseLightMaterial.h>

#define BOOK1_SCENE_COVER 0
#define BOOK2_SCENE_SIMPLELIGHT 1
#define BOOK2_SCENE_CORNELLBOX 2
#define BOOK2_SCENE_CORNELLBOXSMOKE 3
#define BOOK2_SCENE_FINAL 4

#define ACTIVE_SCENE BOOK2_SCENE_FINAL

ListTraceable set_scene(const int &scene_id, Camera &camera, SolidColorTexture &background, int32_t &samples_per_pixel);

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

	/* auto render_target = RenderTargetPPMImage(image_width, image_height); */
	auto render_target = RenderTargetPNGImage(image_width, image_height);
	/* render_target.setOutFile("image.ppm"); */
	render_target.setOutFile("png_test.png");

	// SceneDielectricMaterial
	ListTraceable list;

	auto material_ground = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.0, 0.0, 0.0), Color(1.0, 1.0, 1.0), 10.0));
	/* auto material_center = std::make_shared<LambertianMaterial>(Color(0.1, 0.2, 0.5)); */
	auto material_center = std::make_shared<LambertianMaterial>(std::make_shared<ImageTexture>("res/earth.jpg"));
	auto material_left   = std::make_shared<DielectricMaterial>(1.5);
	auto material_right  = std::make_shared<MetalMaterial>(Color(0.8, 0.6, 0.2), 0.0);
	auto material_light  = std::make_shared<DiffuseLightMaterial>(Color(40.0, 40.0, 32.0));

	/* list.add(std::make_shared<SphereTraceable>(Point3( 0.0, -101.0, -1.0), 100.0, material_ground)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3( 0.0,    0.0, -1.0),   0.5, material_center)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0),   0.5, material_left)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3(-1.0,    0.0, -1.0), -0.45, material_left)); */
	/* list.add(std::make_shared<SphereTraceable>(Point3( 1.0,    0.0, -1.0),   0.5, material_right)); */

	/* list.add(std::make_shared<MovingSphereTraceable>(Point3(1.0, 0.0, 1.0), Point3(-1.0, 0.0, 1.0), 0.0, 0.2, 0.5, material_center)); */

	/* list.add(std::make_shared<SphereTraceable>(Point3( -2.0, 1.0, 0.0), 0.4, material_light)); */
	/* auto material_superlight  = std::make_shared<DiffuseLightMaterial>(Color(4.0, 4.0, 3.0)); */
	//list.add(std::make_shared<SphereTraceable>(Point3( 70.0, 200.0, 50.0), 20, material_superlight));

	/* list.add(std::make_shared<StaticMeshTraceable>("res/IronMan2_PBR.obj", "res")); */
	/* list.add(std::make_shared<GLTFStaticMeshTraceable>("res/DamagedHelmet2/DamagedHelmet2.gltf", "res/DamagedHelmet2")); */
	/* list.add(std::make_shared<GLTFStaticMeshTraceable>("res/Duck/Duck.gltf", "res/Duck")); */
	/* list.add(std::make_shared<GLTFStaticMeshTraceable>("res/BoxTextured/BoxTextured.gltf", "res/BoxTextured")); */
	/* list.add(std::make_shared<GLTFStaticMeshTraceable>("res/Box/Box.gltf", "res/Box")); */
	/* list.add(std::make_shared<SphereTraceable>(Point3(30.0, 200, 50), 20.0, material_right)); */
	Camera camera;
	SolidColorTexture background;
	int32_t samples_per_pixel;
	list = set_scene(ACTIVE_SCENE, camera, background, samples_per_pixel);

	BVHNode bvh(list, 0.0f, 0.0f);
	/* print_bvh(&bvh, 0); */
	/* exit(1); */

	// Render
	const int32_t max_ray_depth = 50;

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

ListTraceable book1_cover_scene()
{
  ListTraceable world;

  auto ground_material = std::make_shared<LambertianMaterial>(Color(0.5));
  world.add(std::make_shared<SphereTraceable>(Point3(0,-1000,0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
      for (int b = -11; b < 11; b++) {
          auto choose_mat = random_float();
          Point3 center(a + 0.9*random_float(), 0.2, b + 0.9*random_float());

          if (length(center - Point3(4, 0.2, 0)) > 0.9) {
						std::shared_ptr<Material> sphere_material;

              if (choose_mat < 0.8) {
                  // diffuse
                  auto albedo = Color::fromVec3f(Vec3f::random()) * Color::fromVec3f(Vec3f::random());
                  sphere_material = std::make_shared<LambertianMaterial>(albedo);
                  world.add(std::make_shared<SphereTraceable>(center, 0.2, sphere_material));
              } else if (choose_mat < 0.95) {
                  // metal
                  auto albedo = Color::fromVec3f(Vec3f::random(0.5, 1));
                  auto fuzz = random_float(0, 0.5);
                  sphere_material = std::make_shared<MetalMaterial>(albedo, fuzz);
                  world.add(std::make_shared<SphereTraceable>(center, 0.2, sphere_material));
              } else {
                  // glass
                  sphere_material = std::make_shared<DielectricMaterial>(1.5);
                  world.add(std::make_shared<SphereTraceable>(center, 0.2, sphere_material));
              }
          }
      }
  }

  auto material1 = std::make_shared<DielectricMaterial>(1.5);
  world.add(std::make_shared<SphereTraceable>(Point3(0, 1, 0), 1.0, material1));

  auto material2 = std::make_shared<LambertianMaterial>(Color(0.4, 0.2, 0.1));
  world.add(std::make_shared<SphereTraceable>(Point3(-4, 1, 0), 1.0, material2));

  auto material3 = std::make_shared<MetalMaterial>(Color(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<SphereTraceable>(Point3(4, 1, 0), 1.0, material3));

  return world;
}

#include <traceables/AARectTraceable.h>
ListTraceable book2_simplelight_scene()
{
	ListTraceable world;
	const auto checkered_texture = std::make_shared<CheckeredTexture>(Color(1.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), 10);
	world.add(std::make_shared<SphereTraceable>(Point3(0, -1000, 0), 1000, std::make_shared<LambertianMaterial>(checkered_texture)));
	world.add(std::make_shared<SphereTraceable>(Point3(0, 2, 0), 2, std::make_shared<LambertianMaterial>(checkered_texture)));

	const auto difflight = std::make_shared<DiffuseLightMaterial>(Color(4.0));
	world.add(std::make_shared<XYRectTraceable>(3, 5, 1, 3, -2, difflight));

	return world;
}

#include <traceables/BoxTraceable.h>
#include <traceables/TranslateTraceable.h>
#include <traceables/RotateYTraceable.h>
ListTraceable book2_cornellbox_scene()
{
	ListTraceable world;

	const auto red = std::make_shared<LambertianMaterial>(Color(0.65, 0.05, 0.05));
	const auto white = std::make_shared<LambertianMaterial>(Color(0.73, 0.73, 0.73));
	const auto green = std::make_shared<LambertianMaterial>(Color(0.12, 0.45, 0.15));
	const auto light = std::make_shared<DiffuseLightMaterial>(Color(15));

	world.add(std::make_shared<YZRectTraceable>(0, 555, 0, 555, 555, green));
	world.add(std::make_shared<YZRectTraceable>(0, 555, 0, 555, 0, red));
	world.add(std::make_shared<XZRectTraceable>(213, 343, 227, 332, 554, light));
	world.add(std::make_shared<XZRectTraceable>(0, 555, 0, 555, 0, white));
	world.add(std::make_shared<XZRectTraceable>(0, 555, 0, 555, 555, white));
	world.add(std::make_shared<XYRectTraceable>(0, 555, 0, 555, 555, white));

	std::shared_ptr<Traceable> box1 = std::make_shared<BoxTraceable>(Point3(0.0f), Point3(165.0f, 330.0f, 165.0f), white);
	box1 = std::make_shared<RotateYTraceable>(box1, 15.0f);
	box1 = std::make_shared<TranslateTraceable>(box1, Vec3f(265.0f, 0.0f, 295.0f));
	world.add(box1);

	std::shared_ptr<Traceable> box2 = std::make_shared<BoxTraceable>(Point3(0.0f), Point3(165.0f), white);
	box2 = std::make_shared<RotateYTraceable>(box2, -18.0f);
	box2 = std::make_shared<TranslateTraceable>(box2, Vec3f(130.0f, 0.0f, 65.0f));
	world.add(box2);

	return world;
}

#include <traceables/VolumeTraceable.h>
ListTraceable book2_cornellboxsmoke_scene()
{
	ListTraceable world;

	const auto red = std::make_shared<LambertianMaterial>(Color(0.65, 0.05, 0.05));
	const auto white = std::make_shared<LambertianMaterial>(Color(0.73, 0.73, 0.73));
	const auto green = std::make_shared<LambertianMaterial>(Color(0.12, 0.45, 0.15));
	const auto light = std::make_shared<DiffuseLightMaterial>(Color(15));

	world.add(std::make_shared<YZRectTraceable>(0, 555, 0, 555, 555, green));
	world.add(std::make_shared<YZRectTraceable>(0, 555, 0, 555, 0, red));
	world.add(std::make_shared<XZRectTraceable>(113, 443, 127, 432, 554, light));
	world.add(std::make_shared<XZRectTraceable>(0, 555, 0, 555, 0, white));
	world.add(std::make_shared<XZRectTraceable>(0, 555, 0, 555, 555, white));
	world.add(std::make_shared<XYRectTraceable>(0, 555, 0, 555, 555, white));

	std::shared_ptr<Traceable> box1 = std::make_shared<BoxTraceable>(Point3(0.0f), Point3(165.0f, 330.0f, 165.0f), white);
	box1 = std::make_shared<RotateYTraceable>(box1, 15.0f);
	box1 = std::make_shared<TranslateTraceable>(box1, Vec3f(265.0f, 0.0f, 295.0f));
	box1 = std::make_shared<VolumeTraceable>(box1, 0.01, Color(0.0f));
	world.add(box1);

	std::shared_ptr<Traceable> box2 = std::make_shared<BoxTraceable>(Point3(0.0f), Point3(165.0f), white);
	box2 = std::make_shared<RotateYTraceable>(box2, -18.0f);
	box2 = std::make_shared<TranslateTraceable>(box2, Vec3f(130.0f, 0.0f, 65.0f));
	box2 = std::make_shared<VolumeTraceable>(box2, 0.01, Color(1.0f));
	world.add(box2);

	return world;
}

ListTraceable book2_final_scene()
{
	ListTraceable boxes1;
	auto ground = std::make_shared<LambertianMaterial>(Color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++) {
			for (int j = 0; j < boxes_per_side; j++) {
					auto w = 100.0;
					auto x0 = -1000.0 + i*w;
					auto z0 = -1000.0 + j*w;
					auto y0 = 0.0;
					auto x1 = x0 + w;
					auto y1 = random_float(1,101);
					auto z1 = z0 + w;

					boxes1.add(std::make_shared<BoxTraceable>(Point3(x0,y0,z0), Point3(x1,y1,z1), ground));
			}
	}

	ListTraceable objects;

	objects.add(std::make_shared<BVHNode>(boxes1, 0, 1));

	auto light = std::make_shared<DiffuseLightMaterial>(Color(7, 7, 7));
	objects.add(std::make_shared<XZRectTraceable>(123, 423, 147, 412, 554, light));

	auto center1 = Point3(400, 400, 200);
	auto center2 = center1 + Vec3f(30,0,0);
	auto moving_sphere_material = std::make_shared<LambertianMaterial>(Color(0.7, 0.3, 0.1));
	objects.add(std::make_shared<MovingSphereTraceable>(center1, center2, 0, 1, 50, moving_sphere_material));

	objects.add(std::make_shared<SphereTraceable>(Point3(260, 150, 45), 50, std::make_shared<DielectricMaterial>(1.5)));
	objects.add(std::make_shared<SphereTraceable>(
			Point3(0, 150, 145), 50, std::make_shared<MetalMaterial>(Color(0.8, 0.8, 0.9), 1.0)
	));

	auto boundary = std::make_shared<SphereTraceable>(Point3(360,150,145), 70, std::make_shared<DielectricMaterial>(1.5));
	objects.add(boundary);
	objects.add(std::make_shared<VolumeTraceable>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = std::make_shared<SphereTraceable>(Point3(0, 0, 0), 5000, std::make_shared<DielectricMaterial>(1.5));
	objects.add(std::make_shared<VolumeTraceable>(boundary, .0001, Color(1,1,1)));

	auto emat = std::make_shared<LambertianMaterial>(std::make_shared<ImageTexture>("res/earth.jpg"));
	objects.add(std::make_shared<SphereTraceable>(Point3(400,200,400), 100, emat));
	/* auto pertext = std::make_shared<NoiseTexture>(0.1); */
	/* objects.add(std::make_shared<SphereTraceable>(Point3(220,280,300), 80, std::make_shared<LambertianMaterial>(pertext))); */

	ListTraceable boxes2;
	auto white = std::make_shared<LambertianMaterial>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
			boxes2.add(std::make_shared<SphereTraceable>(Point3::random(0,165), 10, white));
	}

	objects.add(std::make_shared<TranslateTraceable>(
				std::make_shared<RotateYTraceable>(
				std::make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
				Vec3f(-100,270,395)
			)
	);

	return objects;
}

ListTraceable set_scene(const int &scene_id, Camera &camera, SolidColorTexture &background, int32_t &samples_per_pixel)
{
	switch(scene_id)
	{
		case BOOK1_SCENE_COVER:
			{
			const Vec3f look_from(13.0f, 2.0f, 3.0f);
			const Vec3f look_at(0.0f, 0.0f, 0.0f);
			const Vec3f up(0.0f, 1.0f, 0.0f);
			const auto aspect_ratio = real(16.0 / 9.0);
			const real fov = 60;
			const real aperture = 0.1;
			const auto dist_to_focus = 10.0f;
			real frameTime0 = 0.0;
			real frameTime1 = 1.0;

			background = SolidColorTexture(Color(0.5));
			samples_per_pixel = 500;

			camera = Camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

			return book1_cover_scene();
			/* break; */
			}
		case BOOK2_SCENE_SIMPLELIGHT:
			{

			samples_per_pixel = 400;
			background = SolidColorTexture(Color(0.0));

			const Vec3f look_from(26.0f, 3.0f, 6.0f);
			const Vec3f look_at(0.0f, 2.0f, 0.0f);
			const Vec3f up(0.0f, 1.0f, 0.0f);
			const auto aspect_ratio = real(16.0 / 9.0);
			const real fov = 20;
			const real aperture = 0.1;
			const auto dist_to_focus = length(look_at - look_from);
			real frameTime0 = 0.0;
			real frameTime1 = 1.0;

			camera = Camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

			return book2_simplelight_scene();
			/* break; */
			}
		case BOOK2_SCENE_CORNELLBOX:
			{

				samples_per_pixel = 200;
				background = SolidColorTexture(Color(0.0));

				const Vec3f look_from(278, 278, -800);
				const Vec3f look_at(278, 278, 0);
				const Vec3f up(0.0f, 1.0f, 0.0f);
				const auto aspect_ratio = real(16.0 / 9.0);
				const real fov = 40;
				const real aperture = 0.1;
				const auto dist_to_focus = length(look_at - look_from);
				real frameTime0 = 0.0;
				real frameTime1 = 1.0;

				camera = Camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

				return book2_cornellbox_scene();
				/* break; */
			}
		case BOOK2_SCENE_CORNELLBOXSMOKE:
			{

				samples_per_pixel = 200;
				background = SolidColorTexture(Color(0.0));

				const Vec3f look_from(278, 278, -800);
				const Vec3f look_at(278, 278, 0);
				const Vec3f up(0.0f, 1.0f, 0.0f);
				const auto aspect_ratio = real(16.0 / 9.0);
				const real fov = 40;
				const real aperture = 0.1;
				const auto dist_to_focus = length(look_at - look_from);
				real frameTime0 = 0.0;
				real frameTime1 = 1.0;

				camera = Camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

				return book2_cornellboxsmoke_scene();
				/* break; */
			}
		case BOOK2_SCENE_FINAL:
			{
				samples_per_pixel = 10000;
				background = Color(0.0f);

				const Vec3f look_from(478, 278, -600);
				const Vec3f look_at(278, 278, 0);
				const Vec3f up(0.0f, 1.0f, 0.0f);
				const auto aspect_ratio = real(16.0 / 9.0);
				const real fov = 40;
				const real aperture = 0.1;
				const auto dist_to_focus = length(look_at - look_from);
				real frameTime0 = 0.0;
				real frameTime1 = 1.0;

				camera = Camera(look_from, look_at, up, fov, aspect_ratio, aperture, dist_to_focus, frameTime0, frameTime1);

				return book2_final_scene();
				/* break; */
			}
		default:
			return ListTraceable();
			break;
	}
}
