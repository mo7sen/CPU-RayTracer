#pragma once
#include <traceables/BVHNode.h>
#include <traceables/StaticMeshTraceable.h>

#include <vector>

class GLTFStaticMeshTraceable : public StaticMeshTraceable
{
public:
	GLTFStaticMeshTraceable(std::string path, std::string res_path = ".", real time0 = 0.0, real time1 = 0.0);
};

