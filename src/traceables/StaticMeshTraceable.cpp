#include <traceables/StaticMeshTraceable.h>

#include <materials/LambertianMaterial.h>
#include <materials/MetalMaterial.h>
#include <materials/DiffuseLightMaterial.h>

#include <textures/CheckeredTexture.h>
#include <textures/ImageTexture.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <extern/tinyobjloader/tiny_obj_loader.h>

#include <assert.h>

StaticMeshTraceable::StaticMeshTraceable(std::string path, std::string mtl_search_path, real time0, real time1)
{
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = mtl_search_path;
	reader_config.triangulate = true;

	tinyobj::ObjReader reader;
	if(!reader.ParseFromFile(path, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "[ERROR] TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if(!reader.Warning().empty()) {
		std::cout << "[WARNING] TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	std::map<std::string, std::shared_ptr<Material>> mat_map;

	// For each material
	for(auto m : materials)
	{
		std::shared_ptr<Material> mat;
		Vec3f emission(m.emission);

		switch(m.illum)
		{
		case 2:		// Highlight on
			if(hmax(emission) > 0) {
				mat = std::make_shared<DiffuseLightMaterial>(Color(emission.x(), emission.y(), emission.z()));
			} else {
				if(m.diffuse_texname.size()) {
					mat = std::make_shared<LambertianMaterial>(std::make_shared<ImageTexture>((mtl_search_path + '/' + m.diffuse_texname).c_str()));
				} else {
					mat = std::make_shared<LambertianMaterial>(Color(m.diffuse[0], m.diffuse[1], m.diffuse[2]));
				}
			}
			break;

		case 3:		// Reflection on and Ray trace on
			if(m.diffuse_texname.size()) {
				mat = std::make_shared<MetalMaterial>(std::make_shared<ImageTexture>((mtl_search_path + '/' + m.diffuse_texname).c_str()), 1.0 - (m.shininess/500.0));
			} else {
				mat = std::make_shared<MetalMaterial>(Color(m.diffuse[0], m.diffuse[1], m.diffuse[2]), 1.0 - (m.shininess/500.0));
			}
			printf("Material Name: %s, metallic: %f, roughness: %f\n", m.name.c_str(), m.metallic, m.roughness);
			break;

		case 0:		// Color on and Ambient off
		case 1:		// Color on and Ambient on
		case 4:		// Transparency: Glass on
 					// Reflection: Ray trace on
		case 5:		// Reflection: Fresnel on and Ray trace on
		case 6:		// Transparency: Refraction on
 					// Reflection: Fresnel off and Ray trace on
		case 7:		// Transparency: Refraction on
 					// Reflection: Fresnel on and Ray trace on
		case 8:		// Reflection on and Ray trace off
		case 9:		// Transparency: Glass on
 					// Reflection: Ray trace off
		case 10:		// Casts shadows onto invisible surfaces
			printf("Unimplemented Illumination model: %d\n", m.illum);
			exit(1);
		}
		mat_map[m.name] = mat;
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {

		  std::shared_ptr<MeshPrimitive> prim;

		  // Assuming that each primitive will have a single material
		  prim = std::make_shared<MeshPrimitive>(mat_map[materials[shapes[s].mesh.material_ids[0]].name]);
	
	  
		  // Loop over faces(polygon)
		  size_t index_offset = 0;
		  for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				if(shapes[s].mesh.num_face_vertices[f] != 3) {
					std::cerr << "Not a triangle\n";
					exit(1);
				}
				// Loop over vertices in the face.
				for (size_t v = 0; v < 3; v++) {
					  // access to vertex
					  tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					  Vec3f position(0.0);
					  position.setX(attrib.vertices[3*size_t(idx.vertex_index)+0]);
					  position.setY(attrib.vertices[3*size_t(idx.vertex_index)+1]);
					  position.setZ(attrib.vertices[3*size_t(idx.vertex_index)+2]);

					  // Check if `normal_index` is zero or positive. negative = no normal data
					  Vec3f normal(0.0);
					  if (idx.normal_index >= 0) {
						normal.setX(attrib.normals[3*size_t(idx.normal_index)+0]);
						normal.setY(attrib.normals[3*size_t(idx.normal_index)+1]);
						normal.setZ(attrib.normals[3*size_t(idx.normal_index)+2]);
					  }

					  // Check if `texcoord_index` is zero or positive. negative = no texcoord data
					  float uv[2] = { 0.0, 0.0 };
					  if (idx.texcoord_index >= 0) {
						uv[0] = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
						uv[1] = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
					  }

					  prim->addVertex(position, normal, uv);

					  // Optional: vertex colors
					  // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					  // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					  // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
				}
				index_offset += 3;

				// per-face material
				// shapes[s].mesh.material_ids[f];
// 				std::cout << "Diffuse Texture Name: " << materials[shapes[s].mesh.material_ids[f]].diffuse_texname << std::endl;
		  }
		  primitivesList.add(prim);
	}

	// TODO: Populate primitives list
	primitivesBVH = BVHNode(primitivesList, time0, time1);
}

bool StaticMeshTraceable::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	/* return primitivesList.hit(ray, t_min, t_max, hitData); */
	return primitivesBVH.hit(ray, t_min, t_max, hitData);
}

bool StaticMeshTraceable::bounding_box(real time0, real time1, AABB& aabb) const
{
	// return primitivesList.bounding_box(time0, time1, aabb);
	return primitivesBVH.bounding_box(time0, time1, aabb);
}

StaticMeshTraceable::MeshPrimitive::MeshPrimitive()
{
	bounds = AABB(Vec3f(std::numeric_limits<float>::max()), Vec3f(std::numeric_limits<float>::min()));
}

bool StaticMeshTraceable::MeshPrimitive::hit(const Ray& ray, real t_min, real t_max, HitData& hitData) const
{
	const float eps = 1e-6;
	bool hit = false;
	for(size_t i = 0; i < positions.size(); i += 3)
	{
		const Vec3f& v0 = positions[i + 0];
		const Vec3f& v1 = positions[i + 1];
		const Vec3f& v2 = positions[i + 2];

		const Vec3f& n0 = normals[i + 0];
		const Vec3f& n1 = normals[i + 1];
		const Vec3f& n2 = normals[i + 2];

		const Vec3f& t0 = tangents[i + 0];
		const Vec3f& t1 = tangents[i + 1];
		const Vec3f& t2 = tangents[i + 2];

		const Vec3f& bt0 = bitangents[i + 0];
		const Vec3f& bt1 = bitangents[i + 1];
		const Vec3f& bt2 = bitangents[i + 2];

		const Vec3f v0v1 = v1 - v0; 
		const Vec3f v0v2 = v2 - v0; 

		const Vec3f face_normal = cross(v0v1, v0v2);
		
		float N_dot_Dir = dot(face_normal, ray.direction() * -1.0);
		if(N_dot_Dir < 0) { // Backface culling
		  continue;
		}
		if(abs(N_dot_Dir) < eps) { // Testing for parallelism
		  continue;
		}

		const float area = length(face_normal) / 2;

		const float face_idx = floor(i / 3);
		
		const auto uv0 = std::pair<float, float>(uvs[face_idx * 6 + (0ull) + 0ull], uvs[face_idx * 6 + (0ull) + 1ull]);
		const auto uv1 = std::pair<float, float>(uvs[face_idx * 6 + (2ull) + 0ull], uvs[face_idx * 6 + (2ull) + 1ull]);
		const auto uv2 = std::pair<float, float>(uvs[face_idx * 6 + (4ull) + 0ull], uvs[face_idx * 6 + (4ull) + 1ull]);

		float t = dot((ray.origin() - v0), face_normal) / N_dot_Dir;

		// check if the triangle is in behind the ray
		if (t < t_min || t > t_max) continue; // the triangle is behind 
 
		// compute the intersection point using equation 1
		Vec3f P = ray.origin() + t * ray.direction(); 
 
		// Step 2: inside-outside test
		Vec3f C; // vector perpendicular to triangle's plane 
 
		////// edge 0
		Vec3f edge0 = v1 - v0; 
		Vec3f vp0 = P - v0; 
		C = cross(edge0, vp0);
		if (dot(face_normal, C) < 0) continue; // P is on the right side 
 
		////// edge 1
		Vec3f edge1 = v2 - v1; 
		Vec3f vp1 = P - v1; 
		C = cross(edge1, vp1); 
		if (dot(face_normal, C) < 0)  continue; // P is on the right side 
		float u = (length(C) / 2) / area;
 
		////// edge 2
		Vec3f edge2 = v0 - v2; 
		Vec3f vp2 = P - v2; 
		C = cross(edge2, vp2); 
		if (dot(face_normal, C) < 0) continue; // P is on the right side;
		float v = (length(C) / 2) / area;

		hitData.hitPos = P;
		hitData.t = t;
		hitData.material = material;

		hitData.hitNormal =
			n0 *          u     +
			n1 *              v + 
			n2 * ( 1.0f - u - v );
		hitData.hitTangent =
			t0 *          u     +
			t1 *              v + 
			t2 * ( 1.0f - u - v );
		hitData.hitBitangent =
			bt0 *          u     +
			bt1 *              v + 
			bt2 * ( 1.0f - u - v );

		hitData.u = (
				( uv0.first  *      u      ) + 
				( uv1.first  *          v  ) +
				( uv2.first  * (1 - u - v) )
		);

		hitData.v = (
				( uv0.second *      u      ) + 
				( uv1.second *          v  ) +
				( uv2.second * (1 - u - v) )
		);

		hitData.u = fmod(hitData.u, 1.001f);
		hitData.v = 1.0f - fmod(hitData.v, 1.001f);

		t_max = t; // Update to make sure that only nearer triangles can be hit
		hit = true; // this ray hits the triangle 
	}
	return hit;
}

bool StaticMeshTraceable::MeshPrimitive::bounding_box(real time0, real time1, AABB& aabb) const
{
	aabb = bounds;
	return true;
}

void StaticMeshTraceable::MeshPrimitive::addVertex(Vec3f position, Vec3f normal, float uv[2], Vec3f tangent, Vec3f bitangent)
{
	positions.push_back(position);
	normals.push_back(normal);
	uvs.push_back(uv[0]);
	uvs.push_back(uv[1]);
	tangents.push_back(tangent);
	bitangents.push_back(bitangent);

	bounds.v_min = min(bounds.v_min, position);
	bounds.v_max = max(bounds.v_max, position);

	/* printf("%f, %f\n", uv[0], uv[1]); */
}

void StaticMeshTraceable::MeshPrimitive::addVertex(Vec3f position, Vec3f normal, float uv[2])
{
	positions.push_back(position);
	normals.push_back(normal);
	uvs.push_back(uv[0]);
	uvs.push_back(uv[1]);

	bounds.v_min = min(bounds.v_min, position);
	bounds.v_max = max(bounds.v_max, position);

	/* printf("%f, %f\n", uv[0], uv[1]); */
}
