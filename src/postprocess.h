#pragma once
#include <vector>
#include "vec3.h"

namespace Postprocess {
	// Write black outlines into colors where the depth difference to any 8-neighbor exceeds threshold.
	// depths[i] = INF (or very large) means background.
	/// @brief 应用基于深度的边缘描边效果
	/// @param colors 颜色缓冲区
	/// @param depths 深度缓冲区
	/// @param width 图像宽度
	/// @param height 图像高度
	/// @param threshold 深度阈值
	/// @param outlineColor 描边颜色（默认黑色）
	void applyDepthEdgeOutline(std::vector<Vec3>& colors,
		const std::vector<double>& depths,
		int width, int height,
		double threshold,
		const Vec3& outlineColor = Vec3(0, 0, 0));
}


