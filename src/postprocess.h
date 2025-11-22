#pragma once
#include <vector>
#include "vec3.h"

namespace Postprocess {
	// Write black outlines into colors where the depth difference to any 8-neighbor exceeds threshold.
	// depths[i] = INF (or very large) means background.
	void applyDepthEdgeOutline(std::vector<Vec3>& colors,
		const std::vector<double>& depths,
		int width, int height,
		double threshold,
		const Vec3& outlineColor = Vec3(0, 0, 0));
}


