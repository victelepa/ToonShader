#include "renderer.h"
#include "postprocess.h"
#include <limits>
#include <fstream>
#include <iostream>

Renderer::Renderer(int w, int h, const Camera& cam, const Light& l)
	: width(w), height(h), camera(cam), light(l) {}

bool Renderer::renderPPM(const std::vector<std::shared_ptr<Hittable>>& objects,
	const ToonParams& toonParams,
	const std::string& outputPath,
	bool enableDepthEdges,
	double depthEdgeThreshold) {
	const double INF = std::numeric_limits<double>::infinity();
	std::vector<Vec3> colorBuffer(width * height, Vec3(0, 0, 0));
	std::vector<double> depthBuffer(width * height, INF);

	auto idx = [this](int x, int y) { return y * this->width + x; };

	std::cout << "Rendering " << width << "x" << height << " image...\n";
	for (int y = 0; y < height; ++y) {
		if (y % 50 == 0) {
			std::cout << "Progress: " << (y * 100 / height) << "%\n";
		}
		double v = (double(y) + 0.5) / double(height);
		for (int x = 0; x < width; ++x) {
			double u = (double(x) + 0.5) / double(width);
			Ray r = camera.get_ray(u, 1.0 - v); // flip v so image isn't upside down

			double t_min = 1e-4;
			double t_max = INF;
			HitRecord closestHit;
			bool hitSomething = false;
			for (const auto& obj : objects) {
				HitRecord rec;
				if (obj->hit(r, t_min, t_max, rec)) {
					hitSomething = true;
					t_max = rec.t;
					closestHit = rec;
				}
			}

			int i = idx(x, y);
			if (hitSomething) {
				// View direction is from point to camera
				Vec3 viewDir = ( - r.direction ).normalized();
				Vec3 c = ToonShader::shade(closestHit, viewDir, light, toonParams);
				colorBuffer[i] = c;
				depthBuffer[i] = closestHit.t;
			}
			else {
				// Sky/background: flat color
				colorBuffer[i] = Vec3(0.8, 0.9, 1.0) * 0.95;
				depthBuffer[i] = INF;
			}
		}
	}
	std::cout << "Progress: 100%\n";

	if (enableDepthEdges) {
		std::cout << "Applying depth edge detection...\n";
		Postprocess::applyDepthEdgeOutline(colorBuffer, depthBuffer, width, height, depthEdgeThreshold, Vec3(1.0, 0.0, 0.0)); // Bright red outline
	}

	writePPM(colorBuffer, outputPath);
	return true;
}

void Renderer::writePPM(const std::vector<Vec3>& colors, const std::string& path) const {
	std::ofstream out(path, std::ios::binary);
	if (!out.is_open()) {
		std::cerr << "Failed to open output PPM: " << path << "\n";
		return;
	}
	out << "P3\n" << width << " " << height << "\n255\n";
	for (int i = 0; i < width * height; ++i) {
		Vec3 c = Vec3::clamp01(colors[i]);
		int ir = static_cast<int>(255.999 * c.x);
		int ig = static_cast<int>(255.999 * c.y);
		int ib = static_cast<int>(255.999 * c.z);
		out << ir << " " << ig << " " << ib << "\n";
	}
}


