#include "mesh_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace {
	static bool parseFaceVertex(const std::string& token, int& outIndex) {
		// token formats: "i", "i/j", "i//k", "i/j/k"
		// we only need the first index (position)
		if (token.empty()) return false;
		std::stringstream ss(token);
		std::string idxStr;
		if (!std::getline(ss, idxStr, '/')) return false;
		try {
			outIndex = std::stoi(idxStr);
		}
		catch (...) {
			return false;
		}
		return true;
	}
}

bool MeshLoader::loadOBJ(const std::string& path,
	double uniformScale,
	const Vec3& translate,
	const Material& material,
	std::vector<std::shared_ptr<Hittable>>& outObjects) {
	std::ifstream in(path);
	if (!in.is_open()) {
		std::cerr << "Failed to open OBJ: " << path << "\n";
		return false;
	}
	std::vector<Vec3> positions;
	std::string line;
	while (std::getline(in, line)) {
		if (line.empty()) continue;
		std::istringstream iss(line);
		std::string tag;
		iss >> tag;
		if (tag == "v") {
			double x, y, z;
			if (!(iss >> x >> y >> z)) continue;
			Vec3 p(x, y, z);
			positions.push_back(p);
		}
		else if (tag == "f") {
			std::vector<int> faceIdx;
			std::string tok;
			while (iss >> tok) {
				int idx = 0;
				if (parseFaceVertex(tok, idx)) {
					if (idx < 0) idx = int(positions.size()) + idx + 1; // handle negative indices
					faceIdx.push_back(idx);
				}
			}
			if (faceIdx.size() < 3) continue;
			// Fan triangulation
			int i0 = faceIdx[0] - 1;
			for (size_t k = 1; k + 1 < faceIdx.size(); ++k) {
				int i1 = faceIdx[k] - 1;
				int i2 = faceIdx[k + 1] - 1;
				if (i0 < 0 || i1 < 0 || i2 < 0) continue;
				if (i0 >= (int)positions.size() || i1 >= (int)positions.size() || i2 >= (int)positions.size()) continue;
				Vec3 a = positions[i0] * uniformScale + translate;
				Vec3 b = positions[i1] * uniformScale + translate;
				Vec3 c = positions[i2] * uniformScale + translate;
				outObjects.push_back(std::make_shared<Triangle>(a, b, c, material));
			}
		}
	}
	return true;
}


