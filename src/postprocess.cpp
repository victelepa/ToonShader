#include "postprocess.h"
#include <cmath>
#include <limits>

void Postprocess::applyDepthEdgeOutline(std::vector<Vec3>& colors,
	const std::vector<double>& depths,
	int width, int height,
	double threshold,
	const Vec3& outlineColor) {
	if ((int)colors.size() != width * height || (int)depths.size() != width * height) return;

	const double INF = std::numeric_limits<double>::infinity();

	auto idx = [width](int x, int y) { return y * width + x; };
	std::vector<char> isEdge(width * height, 0);

	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			int i = idx(x, y);
			double d = depths[i];
			bool edge = false;
			for (int oy = -1; oy <= 1 && !edge; ++oy) {
				for (int ox = -1; ox <= 1 && !edge; ++ox) {
					if (ox == 0 && oy == 0) continue;
					double dn = depths[idx(x + ox, y + oy)];
					if (std::isinf(d) && std::isinf(dn)) continue;
					// If either is inf and the other is finite -> edge
					if ((std::isinf(d) && !std::isinf(dn)) || (!std::isinf(d) && std::isinf(dn))) {
						edge = true;
						break;
					}
					// Otherwise compare absolute difference
					if (std::fabs(d - dn) > threshold) {
						edge = true;
						break;
					}
				}
			}
			if (edge) isEdge[i] = 1;
		}
	}

	for (int i = 0; i < width * height; ++i) {
		if (isEdge[i]) {
			colors[i] = outlineColor;
		}
	}
}


