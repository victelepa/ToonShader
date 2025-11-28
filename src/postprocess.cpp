#include "postprocess.h"
#include <cmath>
#include <limits>

void Postprocess::applyDepthEdgeOutline(std::vector<Vec3>& colors,
	const std::vector<double>& depths,
	int width, int height,
	double threshold,
	const Vec3& outlineColor) {
	
	// 如果颜色或深度缓冲区大小不匹配，直接返回
	if ((int)colors.size() != width * height || (int)depths.size() != width * height) return;

	const double INF = std::numeric_limits<double>::infinity();

	// Lambda函数：根据(x,y)坐标计算一维数组索引
	/// @param x 像素x坐标
	/// @param y 像素y坐标
	/// @return 对应于(x,y)的一维数组索引
	auto idx = [width](int x, int y) { return y * width + x; };

	// 标记边缘像素的数组
	std::vector<char> isEdge(width * height, 0);

	// Sobel operator kernels for edge detection
	// Gx (horizontal gradient):
	// [-1  0  1]
	// [-2  0  2]
	// [-1  0  1]
	// Gy (vertical gradient):
	// [-1 -2 -1]
	// [ 0  0  0]
	// [ 1  2  1]

	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			int i = idx(x, y);
			double d = depths[i];
			
			// Skip if current pixel is background
			if (std::isinf(d)) continue;

			// Apply Sobel operator
			// Sobel kernels:
			// Gx: [-1  0  1]    Gy: [-1 -2 -1]
			//     [-2  0  2]        [ 0  0  0]
			//     [-1  0  1]        [ 1  2  1]
			
			double gx = 0.0, gy = 0.0;
			
			// Helper function to get depth value (treat INF as 0 for gradient calculation)
			auto getDepth = [&depths, &idx, INF](int px, int py) -> double {
				double val = depths[idx(px, py)];
				return std::isinf(val) ? 0.0 : val;
			};

			// Calculate Sobel Gx (horizontal gradient)
			gx = -1.0 * getDepth(x-1, y-1) +  1.0 * getDepth(x+1, y-1)
			   + -2.0 * getDepth(x-1, y)   +  2.0 * getDepth(x+1, y)
			   + -1.0 * getDepth(x-1, y+1) +  1.0 * getDepth(x+1, y+1);

			// Calculate Sobel Gy (vertical gradient)
			gy = -1.0 * getDepth(x-1, y-1) + -2.0 * getDepth(x, y-1) + -1.0 * getDepth(x+1, y-1)
			   +  1.0 * getDepth(x-1, y+1) +  2.0 * getDepth(x, y+1) +  1.0 * getDepth(x+1, y+1);

			// Calculate gradient magnitude
			double gradientMagnitude = std::sqrt(gx * gx + gy * gy);

			// Check for background edges (if any neighbor is background)
			bool backgroundEdge = false;
			for (int oy = -1; oy <= 1; ++oy) {
				for (int ox = -1; ox <= 1; ++ox) {
					if (ox == 0 && oy == 0) continue;
					double dn = depths[idx(x + ox, y + oy)];
					if (std::isinf(dn)) {
						backgroundEdge = true;
						break;
					}
				}
				if (backgroundEdge) break;
			}

			// Mark as edge if gradient magnitude exceeds threshold or is a background edge
			// For Sobel, we use a higher threshold to get thinner edges
			if (gradientMagnitude > threshold || backgroundEdge) {
				isEdge[i] = 1;
			}
		}
	}

	for (int i = 0; i < width * height; ++i) {
		if (isEdge[i]) {
			colors[i] = outlineColor;
		}
	}
}


