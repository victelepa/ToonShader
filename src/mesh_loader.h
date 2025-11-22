#pragma once
#include <string>
#include <vector>
#include <memory>
#include "hittable.h"
#include "triangle.h"
#include "material.h"

namespace MeshLoader {
	// Loads a .OBJ file with only 'v' and 'f' (triangles/convex polygons). Ignores UVs/normals.
	// Applies uniform scale and translation after loading.
	// Appends triangles to 'outObjects'.
	bool loadOBJ(const std::string& path,
		double uniformScale,
		const Vec3& translate,
		const Material& material,
		std::vector<std::shared_ptr<Hittable>>& outObjects);
}


