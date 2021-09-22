#pragma once
#include <Material.h>
#include <Texture.h>
#include <textures/SolidColorTexture.h>
#include <optional>

class PBRMaterial : public Material
{
public:
    PBRMaterial() {}

    bool scatter(const Ray& ray_in, const HitData& hitData, Color& attenuation, Ray& ray_scattered) const
    {
        /* if(normalMap) */
        /* { */
            // TODO calculate hitNormal (Needs adding TBN matrix in multiple places, I guess)
        /* } */

        Color metallicRoughnessColor = metallicRoughness->value(hitData.u, hitData.v, hitData.hitPos);
        float metallicFactor = metallicRoughnessColor.b();
        float roughnessFactor = metallicRoughnessColor.g();

        Vec3f reflected = reflect(normalize(ray_in.direction()), hitData.hitNormal);
        ray_scattered = Ray(hitData.hitPos, reflected + roughnessFactor * random_vec_in_unit_sphere(), ray_in.time());
        attenuation = baseColor->value(hitData.u, hitData.v, hitData.hitPos) * (roughnessFactor * (1.0f - metallicFactor));

        return (dot(ray_scattered.direction(), hitData.hitNormal) > 0);
    }

    Color emitted(double u, double v, const Point3& p) const
    {
        if(emissiveMap) 
        {
            Vec3f emissiveColor = emissiveMap.value()->value(u, v, p).toVec3f();
            return Color::fromVec3f(emissiveColor * emissiveFactor);
        }
        else
        {
            return Color(0.0f);
        }
    }

public:
    std::shared_ptr<Texture> baseColor;

    std::shared_ptr<Texture> metallicRoughness;

    std::optional<std::shared_ptr<Texture>> normalMap;

    Vec3f emissiveFactor;
    std::optional<std::shared_ptr<Texture>> emissiveMap;
};

