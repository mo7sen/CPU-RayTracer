#pragma once
#include <Traceable.h>
#include "BVHNode.h"

#include <vector>

class StaticMeshTraceable : public Traceable
{
public:
	class MeshPrimitive : public Traceable 
	{
	private:
		std::vector<Vec3f> positions;
		std::vector<Vec3f> normals;
		std::vector<float> uvs;
		std::vector<Vec3f> tangents;
		std::vector<Vec3f> bitangents;
		std::shared_ptr<Material> material;
		AABB bounds;
	public:
		MeshPrimitive();
		MeshPrimitive(std::shared_ptr<Material> m): material(m) 
		{
			bounds = AABB(Vec3f(std::numeric_limits<float>::max()), Vec3f(std::numeric_limits<float>::min()));
		};
		void addVertex(Vec3f position, Vec3f normal, float uv[2]);
		void addVertex(Vec3f position, Vec3f normal, float uv[2], Vec3f tangent, Vec3f bitangent);
		virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
		virtual bool bounding_box(real time0, real time1, AABB& aabb) const override;
	};

protected:
	ListTraceable primitivesList;
	BVHNode primitivesBVH;

public:
	StaticMeshTraceable() = default;
	StaticMeshTraceable(std::string path, std::string res_path = ".", real time0 = 0.0, real time1 = 0.0);

	virtual bool hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const override;
	virtual bool bounding_box(real time0, real time1, AABB& aabb) const override;
};

