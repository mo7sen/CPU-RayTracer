#pragma once
#include "Material.h"
class MetalMaterial :
    public Material
{
public:
    MetalMaterial(const Color& a, const real f) : albedo(a), fuzziness(f<1?f:1) {}

    bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
    {
        Vec3 reflected = reflect(unit_vector(ray_in.direction()), hitData.hitNormal);
        ray_scattered = Ray(hitData.hitPos, reflected + fuzziness * random_vec_in_unit_sphere(), ray_in.time());
        attenuation = albedo;
        return (dot(ray_scattered.direction(), hitData.hitNormal) > 0);
    }

public:
    Color albedo;
    real fuzziness;
};

