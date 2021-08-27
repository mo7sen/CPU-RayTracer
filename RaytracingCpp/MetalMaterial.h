#pragma once
#include "Material.h"
#include "Texture.h"
#include "SolidColorTexture.h"
class MetalMaterial :
    public Material
{
public:
    MetalMaterial(const Color& a, const real f) : albedo(std::make_shared<SolidColorTexture>(a)), fuzziness(f<1?f:1) {}
	MetalMaterial(std::shared_ptr<Texture> a, const real f) : albedo(a), fuzziness(f<1?f:1) {}

    bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
    {
        Vec3f reflected = reflect(normalize(ray_in.direction()), hitData.hitNormal);
        ray_scattered = Ray(hitData.hitPos, reflected + fuzziness * random_vec_in_unit_sphere(), ray_in.time());
        attenuation = albedo->value(hitData.u, hitData.v, hitData.hitPos);
        return (dot(ray_scattered.direction(), hitData.hitNormal) > 0);
    }

public:
    std::shared_ptr<Texture> albedo;
    real fuzziness;
};

