#include "Camera.h"

Ray Camera::get_ray(real s, real t)
{
	Vec3f rd = lens_radius * random_vec_in_unit_disk();
	Vec3f offset = u * rd.x() + v * rd.y();

	Vec3f ray_orig = origin + offset;
	Vec3f ray_dir = lb_corner + (s * horizontal) + (t * vertical) - (ray_orig);
	return Ray(std::move(ray_orig), std::move(ray_dir), random_float(time0, time1));
}
