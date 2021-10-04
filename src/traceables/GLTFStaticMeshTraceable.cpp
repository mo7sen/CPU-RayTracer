#include <traceables/GLTFStaticMeshTraceable.h>

#include <materials/LambertianMaterial.h>
#include <materials/MetalMaterial.h>
#include <materials/DiffuseLightMaterial.h>
#include <materials/PBRMaterial.h>
#include <materials/UVDbgMaterial.h>
#include <materials/NormalDbgMaterial.h>

#include <textures/CheckeredTexture.h>
#include <textures/ImageTexture.h>

#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <assert.h>

GLTFStaticMeshTraceable::GLTFStaticMeshTraceable(std::string path, std::string res_path, real time0, real time1)
{
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);

  if (!warn.empty())
  {
    printf("[TinyGLTF] [WARNING]: %s\n", warn.c_str());
  }
  if (!err.empty())
  {
    printf("[TinyGLTF] [ERROR]: %s\n", err.c_str());
  }

  if (!ret)
  {
    printf("[TinyGLTF] [NOTICE] : Failed to parse file \"%s\"\n", path.c_str());
    exit(1);
  }

  /* std::vector<std::shared_ptr<PBRMaterial>> materials; */
  /* std::vector<std::shared_ptr<UVDbgMaterial>> materials; */
  std::vector<std::shared_ptr<Material>> materials;
  materials.reserve(model.materials.size());
  for(auto& mat : model.materials)
  {
    PBRMaterial m;

    printf("Material: %s\n", mat.name.c_str());

    auto baseColorMapTextureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
    if(baseColorMapTextureIndex != -1)
    {
      auto baseColorMapImageIndex = model.textures[baseColorMapTextureIndex].source;
      auto baseColorMapURI = res_path + "/" + model.images[baseColorMapImageIndex].uri;
      m.baseColor = std::make_shared<ImageTexture>(baseColorMapURI);
      printf("\tFound baseColor Texture\n");
    }
    else
    {
      m.baseColor = std::make_shared<SolidColorTexture>(Color(
            mat.pbrMetallicRoughness.baseColorFactor[0],
            mat.pbrMetallicRoughness.baseColorFactor[1],
            mat.pbrMetallicRoughness.baseColorFactor[2]
            ));
      printf("\tFalling back to baseColor factor\n");
    }

    if(mat.normalTexture.index != -1)
    {
      printf("\tFound normal Texture\n");
      auto normalMapImageIndex = model.textures[mat.normalTexture.index].source;
      auto normalMapURI = res_path + "/" + model.images[normalMapImageIndex].uri;
      m.normalMap = std::make_shared<ImageTexture>(normalMapURI);
    }

    auto metallicRoughnessMapTextureIndex = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
    if(metallicRoughnessMapTextureIndex != -1)
    {
      printf("\tFound metallicRoughness Texture\n");
      auto metallicRoughnessMapImageIndex = model.textures[metallicRoughnessMapTextureIndex].source;
      auto metallicRoughnessMapURI = res_path + "/" + model.images[metallicRoughnessMapImageIndex].uri;
      m.metallicRoughness = std::make_shared<ImageTexture>(metallicRoughnessMapURI);
    }
    else
    {
      float metallicFactor = mat.pbrMetallicRoughness.metallicFactor;
      float roughnessFactor = mat.pbrMetallicRoughness.roughnessFactor;

      m.metallicRoughness = std::make_shared<SolidColorTexture>(Color(1.0, roughnessFactor, metallicFactor));
    }

    m.emissiveFactor = Vec3f(
      mat.emissiveFactor[0],
      mat.emissiveFactor[1],
      mat.emissiveFactor[2]
      );

    if(mat.emissiveTexture.index != -1)
    {
      printf("\tFound emissive Texture\n");
      auto emissiveMapImageIndex = model.textures[mat.emissiveTexture.index].source;
      auto emissiveMapURI = res_path + "/" + model.images[emissiveMapImageIndex].uri;
      m.emissiveMap = std::make_shared<ImageTexture>(emissiveMapURI);
    }

    materials.push_back(std::make_shared<PBRMaterial>(m));
    /* materials.push_back(std::make_shared<NormalDbgMaterial>()); */
    /* materials.push_back(std::make_shared<UVDbgMaterial>()); */
  }

  /* printf("Mesh Count: %lu\n", model.meshes.size()); */
  for(auto& mesh : model.meshes)
  {
    /* printf("Primitives Count: %lu\n", mesh.primitives.size()); */
    for(auto& prim : mesh.primitives)
    {
      auto mesh_prim = std::make_shared<MeshPrimitive>(materials[prim.material]);

      const auto indexAccessor = model.accessors[prim.indices];
      const auto indexBufferView = model.bufferViews[indexAccessor.bufferView];
      const auto indexBuffer = model.buffers[indexBufferView.buffer];
      const auto indexStride = indexAccessor.ByteStride(indexBufferView);
      const auto index_count = indexAccessor.count;
      const uint16_t *indices = reinterpret_cast<const uint16_t *>(indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);
      /* printf("Index Stride: %u\n", indexStride); */

      /* printf("Primitive Index Count: %lu\n", index_count); */
      /* for(int i = 0; i < 30; i++) { */
      /*   printf("Index #%d: %u\n", i, indices[i]); */
      /* } */

      std::vector<Vec3f> positions;
      positions.reserve(index_count);
      std::vector<Vec3f> normals;
      normals.reserve(index_count);
      std::vector<Vec3f> uvs;
      uvs.reserve(index_count);

      std::vector<Vec3f> tangents;
      tangents.reserve(index_count);
      std::vector<Vec3f> bitangents;
      bitangents.reserve(index_count);

      for(auto& attrib : prim.attributes)
      {
        auto attribAccessor = model.accessors[attrib.second];
        auto attribBufferView = model.bufferViews[attribAccessor.bufferView];
        auto attribBuffer = model.buffers[attribBufferView.buffer];
        auto attribStride = attribAccessor.ByteStride(attribBufferView);
        /* printf("Attribute Name: %s, Attribute Count: %u\n", attrib.first.c_str(), uint32_t(attribAccessor.count)); */
        /* printf("Attribute Size: %lu, AttributeStride: %lu\n", attribBufferView.byteLength / attribAccessor.count, attribBufferView.byteStride); */


        for(size_t index_i = 0; index_i < index_count; index_i++)
        {
          uint16_t index = indices[index_i];
          float *vert = reinterpret_cast<float *>(attribBuffer.data.data() + attribBufferView.byteOffset + attribAccessor.byteOffset + (attribStride * index));

          if(attrib.first.compare("POSITION") == 0)
          {
            positions.push_back(Vec3f(vert[0], vert[1], vert[2]));
            /* printf("Pushing Back Position[%u]: (%f, %f, %f)\n", index, vert[0], vert[1], vert[2]); */
          }
          else if(attrib.first.compare("NORMAL") == 0)
          {
            normals.push_back(Vec3f(vert[0], vert[1], vert[2]));
          }
          else if(attrib.first.compare("TEXCOORD_0") == 0)
          {
            uvs.push_back(Vec3f(vert[0], vert[1], vert[0]));
          }
          else if(attrib.first.compare("TANGENT") == 0)
          {
            tangents.push_back(Vec3f(vert[0], vert[1], vert[2]));
            bitangents.push_back(normalize(cross(normals[index_i], tangents[index_i])) * vert[3]);
          }
          else
          {
            break;
          }
        }
      }
      for(size_t index_i = 0; index_i < index_count; index_i++)
      {
        float uv[2] = { uvs[index_i].x(), uvs[index_i].y() };
        /* printf("positions[%lu]: %f, %f, %f\n", index_i, positions[index_i].x(), positions[index_i].y(), positions[index_i].z()); */
        mesh_prim->addVertex(positions[index_i], normals[index_i], uv, tangents[index_i], bitangents[index_i]);
      }
      assert(index_count != 0);
      assert(index_count % 3 == 0);
      primitivesList.add(mesh_prim);
    }
  }

  primitivesBVH = BVHNode(primitivesList, time0, time1);
}
