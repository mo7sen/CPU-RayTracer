#pragma once
#include <Material.h>

class NormalDbgMaterial : public Material
{
public:
    NormalDbgMaterial() {}

    bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
    {
        Vec3f reflected = reflect(normalize(ray_in.direction()), hitData.hitNormal);
        ray_scattered = Ray(hitData.hitPos, reflected +/* roughnessFactor * */random_vec_in_unit_sphere(), ray_in.time());
        auto norm = abs(hitData.hitNormal);
        attenuation = Color(norm.x(), norm.y(), norm.z());
        return (dot(ray_scattered.direction(), hitData.hitNormal) > 0);
    }

    Color emitted(double u, double v, const Point3& p) const
    {
        return Color(0.0f);
    }
};

