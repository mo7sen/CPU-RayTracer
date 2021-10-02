#include <traceables/VolumeTraceable.h>

bool VolumeTraceable::hit(const Ray &ray, real t_min, real t_max, HitData &hitData) const
{
  const bool enableDebug = false;
  const bool debugging = enableDebug && random_float() < 0.00001;

  HitData hit1, hit2;

  if(!boundary->hit(ray, -INF, INF, hit1)) {
    return false;
  }

  if(!boundary->hit(ray, hit1.t + 0.0001, INF, hit2)) {
    return false;
  }

  if (debugging) std::cerr << "\nt_min=" << hit1.t << ", t_max=" << hit2.t << '\n';

  if(hit1.t < t_min) {
    hit1.t = t_min;
  }

  if(hit2.t > t_max) {
    hit2.t = t_max;
  }

  if(hit1.t >= hit2.t) {
    return false;
  }

  if(hit1.t < 0) {
    hit1.t = 0;
  }

  const auto ray_length = length(ray.direction());
  const auto distance_inside_boundary = (hit2.t - hit1.t) * ray_length;
  const auto hit_distance = neg_inv_density * logf(random_float());

  if(hit_distance > distance_inside_boundary) {
    return false;
  }

  hitData.t = hit1.t + hit_distance / ray_length;
  hitData.hitPos = ray.at(hitData.t);

  if(debugging) {
    std::cerr << "hit_distance = " << hit_distance << '\n'
      << "hitData.t = " << hitData.t << '\n'
      << "hitData.hitPos = " << hitData.hitPos << '\n';
  }

  hitData.hitNormal = Vec3f(1.0f, 0.0f, 0.0f);
  hitData.material = phase_function;

  return true;
}
