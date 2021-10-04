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
        Vec3f normal = hitData.hitNormal;
        if(normalMap)
        {
            Color normalColor = normalMap.value()->value(hitData.u, hitData.v, hitData.hitPos);
            Vec3f mapNormal = normalColor.toVec3f() * 2.0f - Vec3f(1.0f);

            Vec3f TBN_R0(hitData.hitTangent.x(), hitData.hitBitangent.x(), hitData.hitNormal.x());
            Vec3f TBN_R1(hitData.hitTangent.y(), hitData.hitBitangent.y(), hitData.hitNormal.y());
            Vec3f TBN_R2(hitData.hitTangent.z(), hitData.hitBitangent.z(), hitData.hitNormal.z());

            normal = Vec3f(
                    dot(TBN_R0, mapNormal),
                    dot(TBN_R1, mapNormal),
                    dot(TBN_R2, mapNormal));

            normal = normalize(normal);
        }

        Color metallicRoughnessColor = metallicRoughness->value(hitData.u, hitData.v, hitData.hitPos);
        float metallicFactor = metallicRoughnessColor.b();
        float roughnessFactor = metallicRoughnessColor.g();

        Vec3f reflected = reflect(normalize(ray_in.direction()), normal);
        ray_scattered = Ray(hitData.hitPos, reflected + roughnessFactor * random_vec_in_unit_sphere(), ray_in.time());
        attenuation = baseColor->value(hitData.u, hitData.v, hitData.hitPos) * (1.0f - (roughnessFactor - metallicFactor));

        return (dot(ray_scattered.direction(), normal) > 0);
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

