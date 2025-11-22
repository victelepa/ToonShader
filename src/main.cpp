#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "material.h"
#include "hittable.h"
#include "sphere.h"
#include "triangle.h"
#include "mesh_loader.h"
#include "renderer.h"
#include "toon_shader.h"

static bool file_exists(const std::string& path) {
	std::ifstream f(path);
	return f.good();
}

int main() {
	// Image settings
	const int width = 640;
	const int height = 360;
	const std::string outputPath = "toon_output.ppm";

	// Camera
	Vec3 lookFrom(2.2, 1.6, 3.8);
	Vec3 lookAt(0, 0.6, 0);
	Vec3 vup(0, 1, 0);
	double vfov = 45.0;
	double aspect = double(width) / double(height);
	Camera cam(lookFrom, lookAt, vup, vfov, aspect);

	// Light (directional)
	Light light;
	light.direction = Vec3(-0.7, -1.0, -0.4).normalized();
	light.color = Vec3(1.0, 1.0, 1.0);

	// Materials
	Material red; red.albedo = Vec3(0.9, 0.25, 0.25); red.shininess = 64.0;
	Material green; green.albedo = Vec3(0.25, 0.9, 0.25); green.shininess = 16.0;
	Material gray; gray.albedo = Vec3(0.6, 0.6, 0.6); gray.shininess = 32.0;

	// Scene objects
	std::vector<std::shared_ptr<Hittable>> objects;

	// Ground as two triangles (a large quad)
	{
		double s = 4.0;
		Vec3 a(-s, 0.0, -s);
		Vec3 b(s, 0.0, -s);
		Vec3 c(s, 0.0, s);
		Vec3 d(-s, 0.0, s);
		objects.push_back(std::make_shared<Triangle>(a, b, c, gray));
		objects.push_back(std::make_shared<Triangle>(a, c, d, gray));
	}

	// Sphere
	objects.push_back(std::make_shared<Sphere>(Vec3(0.0, 0.6, 0.0), 0.6, red));

	// Optional OBJ mesh (if a file "model.obj" exists, load and place it)
	{
		std::string objPath = "model.obj";
		if (file_exists(objPath)) {
			MeshLoader::loadOBJ(objPath, /*scale=*/1.0, /*translate=*/Vec3(-1.0, 0.0, 0.0), green, objects);
			std::cout << "Loaded OBJ: " << objPath << "\n";
		}
		else {
			std::cout << "OBJ not found (" << objPath << "), continuing without it.\n";
		}
	}

	// Toon parameters with a warm ramp
	ToonParams toon;
	toon.diffuseBands = 3;
	toon.silhouetteThreshold = 0.25; // stronger outlines
	toon.specularThreshold1 = 0.55;
	toon.specularThreshold2 = 0.25;
	toon.specColorA = Vec3(1.0, 1.0, 1.0);
	toon.specColorB = Vec3(0.8, 0.8, 0.8);
	toon.rampColors = {
		Vec3(0.15, 0.10, 0.10), // darkest band
		Vec3(0.80, 0.55, 0.35), // mid band
		Vec3(1.00, 0.90, 0.60)  // light band
	};

	Renderer renderer(width, height, cam, light);
	bool enableDepthEdges = true;
	double depthEdgeThreshold = 0.05; // tweak to control outline from depth discontinuities

	if (renderer.renderPPM(objects, toon, outputPath, enableDepthEdges, depthEdgeThreshold)) {
		std::cout << "Wrote: " << outputPath << "\n";
	}
	else {
		std::cerr << "Render failed.\n";
	}
	return 0;
}


