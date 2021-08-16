#include "Camera.h"

Ray Camera::get_ray(real s, real t)
{
	Vec3 rd = lens_radius * random_vec_in_unit_disk();
	Vec3 offset = u * rd.x() + v * rd.y();
	return Ray(origin + offset, lb_corner + s*horizontal + t*vertical - origin - offset, random_double(time0, time1));
}
