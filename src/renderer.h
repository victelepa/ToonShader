#pragma once
#include <vector>
#include <memory>
#include <string>
#include "hittable.h"
#include "camera.h"
#include "toon_shader.h"

class Renderer {
public:
	Renderer(int w, int h, const Camera& cam, const Light& light);

	// Renders scene into a color buffer and depth buffer, applies optional depth-edge outlining, and writes PPM.
	bool renderPPM(const std::vector<std::shared_ptr<Hittable>>& objects,
		const ToonParams& toonParams,
		const std::string& outputPath,
		bool enableDepthEdges,
		double depthEdgeThreshold);

private:
	int width;
	int height;
	Camera camera;
	Light light;

	void writePPM(const std::vector<Vec3>& colors, const std::string& path) const;
};


