#include "mesh_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace {
	/// @brief 解析面顶点索引
	/// @param token 面顶点索引字符串
	/// @param outIndex 输出顶点索引
	/// @return 是否成功解析
	static bool parseFaceVertex(const std::string& token, int& outIndex) {
		// token formats: "i", "i/j", "i//k", "i/j/k"
		// we only need the first index (position)
		if (token.empty()) return false;
		std::stringstream ss(token);
		std::string idxStr;

		 // 以 '/' 为分隔，取第一个字段作为顶点索引字符串
		if (!std::getline(ss, idxStr, '/')) return false;
		try 
		{
			// 将顶点索引字符串转换为整数
			outIndex = std::stoi(idxStr);
		}
		catch (...) {
			return false;
		}
		return true;
	}
}


bool MeshLoader::loadOBJ(
	const std::string& path,
	double uniformScale,
	const Vec3& translate,
	const Material& material,
	std::vector<std::shared_ptr<Hittable>>& outObjects) {
	
	/// 打开OBJ文件
	std::ifstream in(path);
	// 检查文件是否成功打开
	if (!in.is_open()) {
		std::cerr << "Failed to open OBJ: " << path << "\n";
		return false;
	}

	/// @brief 存储顶点位置的列表
	std::vector<Vec3> positions;

	/// @brief 当前行内容
	std::string line;
	while (std::getline(in, line)) {
		// 跳过空行
		if (line.empty()) continue;

		// 获取行的第一个单词作为标签
		std::istringstream iss(line);
		std::string tag;
		iss >> tag;

		// 如果标签为V，解析顶点位置
		if (tag == "v") {
			double x, y, z;
			if (!(iss >> x >> y >> z)) continue;
			Vec3 p(x, y, z);
			positions.push_back(p);
		}
		// 如果标签为F，解析面定义
		else if (tag == "f") {
			/// @brief 存储面的顶点索引
			std::vector<int> faceIdx;
			std::string tok;

			// 解析面的每个顶点索引
			while (iss >> tok) {
				int idx = 0;
				if (parseFaceVertex(tok, idx)) {
					// 处理负索引
					if (idx < 0) idx = int(positions.size()) + idx + 1; // handle negative indices
					faceIdx.push_back(idx);
				}
			}
			if (faceIdx.size() < 3) continue;
			// Fan triangulation
			int i0 = faceIdx[0] - 1; // OBJ 索引从 1 开始，转换为 0 基
			for (size_t k = 1; k + 1 < faceIdx.size(); ++k) {
				int i1 = faceIdx[k] - 1;
				int i2 = faceIdx[k + 1] - 1;
				if (i0 < 0 || i1 < 0 || i2 < 0) continue;
				// 如果索引超出范围则跳过
				if (i0 >= (int)positions.size() || i1 >= (int)positions.size() || i2 >= (int)positions.size()) continue;
				// 应用缩放和平移变换后创建三角形并添加到输出对象列表
				Vec3 a = positions[i0] * uniformScale + translate;
				Vec3 b = positions[i1] * uniformScale + translate;
				Vec3 c = positions[i2] * uniformScale + translate;

				// 创建三角形并添加到输出对象列表
				outObjects.push_back(std::make_shared<Triangle>(a, b, c, material));
			}
		}
	}
	return true;
}


