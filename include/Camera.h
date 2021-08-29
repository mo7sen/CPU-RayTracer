#pragma once

#include <common.h>
#include <Point3.h>
#include <Ray.h>

class Camera
{
public:
	Camera(
		Point3 look_from,
		Point3 look_at,
		Vec3f up,
		real vfov_in_deg, 
		real aspect_ratio,
		real aperture,
		real focus_dist,
		double _time0 = 0.0,
		double _time1 = 0.0
	)
	{
		real vfov_in_rad = deg2rad(vfov_in_deg);
		real hfov_in_rad = tan(vfov_in_rad / 2);

		auto viewport_height = 2.0 * hfov_in_rad;
		auto viewport_width = viewport_height * aspect_ratio;

		w = normalize(look_from - look_at);
		u = normalize(cross(up, w));
		v = cross(w, u);

		origin = look_from;
		horizontal = u * viewport_width * focus_dist;
		vertical = v * viewport_height * focus_dist;
		lb_corner = origin - horizontal / 2.0 - vertical / 2.0 - focus_dist * w;
		lens_radius = aperture / 2;

		time0 = _time0;
		time1 = _time1;
	}

	Ray get_ray(real s, real t);
	~Camera() = default;

private:
	Point3 origin;
	Point3 lb_corner;
	Vec3f horizontal;
	Vec3f vertical;
	Vec3f w, u, v;
	real lens_radius;
	double time0; // Shutter Open Time
	double time1; // Shutter Close Time
};

