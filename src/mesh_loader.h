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
	/// @brief 加载OBJ文件并将其转换为可击中对象
	/// @param path OBJ文件路径
	/// @param uniformScale 统一缩放比例
	/// @param translate 平移向量
	/// @param material 材质
	/// @param outObjects 输出可击中对象列表
	/// @return 是否成功加载OBJ文件
	bool loadOBJ(
		const std::string& path,
		double uniformScale,
		const Vec3& translate,
		const Material& material,
		std::vector<std::shared_ptr<Hittable>>& outObjects);
}


