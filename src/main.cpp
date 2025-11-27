#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
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

static Vec3 parseVec3(const std::string& str) {
	std::istringstream iss(str);
	double x, y, z;
	char c1, c2;
	if (iss >> x >> c1 >> y >> c2 >> z && c1 == ',' && c2 == ',') {
		return Vec3(x, y, z);
	}
	std::cerr << "Warning: Invalid Vec3 format '" << str << "', using (0,0,0)\n";
	return Vec3(0, 0, 0);
}

static void printUsage(const char* progName) {
	std::cout << "Usage: " << progName << " [OPTIONS]\n";
	std::cout << "Options:\n";
	std::cout << "  --obj, -o PATH           OBJ file path (default: Cone.obj)\n";
	std::cout << "  --lookFrom, -from X,Y,Z  Camera position (default: 4,4,4)\n";
	std::cout << "  --lookAt, -at X,Y,Z      Camera target (default: 0,0,0)\n";
	std::cout << "  --vfov, -fov DEGREES     Vertical field of view (default: 45.0)\n";
	std::cout << "  --scale, -s VALUE        Object scale (default: 0.7)\n";
	std::cout << "  --translate, -t X,Y,Z    Object translation (default: 1,0.3,1)\n";
	std::cout << "  --help, -h               Show this help message\n";
}

int main(int argc, char* argv[]) {
	// Default values
	std::string objPath = "Cone.obj";
	Vec3 lookFrom(4, 4, 4);
	Vec3 lookAt(0, 0.0, 0);
	double vfov = 45.0;
	double scale = 0.7;
	Vec3 translate(1, 0.3, 1);

	// Parse command line arguments
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--help" || arg == "-h") {
			printUsage(argv[0]);
			return 0;
		}
		else if (arg == "--obj" || arg == "-o") {
			if (i + 1 < argc) {
				objPath = argv[++i];
			} else {
				std::cerr << "Error: --obj requires a path argument\n";
				return 1;
			}
		}
		else if (arg == "--lookFrom" || arg == "-from") {
			if (i + 1 < argc) {
				lookFrom = parseVec3(argv[++i]);
			} else {
				std::cerr << "Error: --lookFrom requires X,Y,Z argument\n";
				return 1;
			}
		}
		else if (arg == "--lookAt" || arg == "-at") {
			if (i + 1 < argc) {
				lookAt = parseVec3(argv[++i]);
			} else {
				std::cerr << "Error: --lookAt requires X,Y,Z argument\n";
				return 1;
			}
		}
		else if (arg == "--vfov" || arg == "-fov") {
			if (i + 1 < argc) {
				vfov = std::stod(argv[++i]);
			} else {
				std::cerr << "Error: --vfov requires a number argument\n";
				return 1;
			}
		}
		else if (arg == "--scale" || arg == "-s") {
			if (i + 1 < argc) {
				scale = std::stod(argv[++i]);
			} else {
				std::cerr << "Error: --scale requires a number argument\n";
				return 1;
			}
		}
		else if (arg == "--translate" || arg == "-t") {
			if (i + 1 < argc) {
				translate = parseVec3(argv[++i]);
			} else {
				std::cerr << "Error: --translate requires X,Y,Z argument\n";
				return 1;
			}
		}
		else {
			std::cerr << "Unknown option: " << arg << "\n";
			std::cerr << "Use --help for usage information\n";
			return 1;
		}
	}
	// Image settings
	const int width = 640;
	const int height = 360;
	const std::string outputPath = "toon_output.ppm";

	// Camera (using command line parameters)
	Vec3 look = (lookAt - lookFrom).normalized();
	Vec3 u = Vec3::cross(look, Vec3(0,1,0)).normalized();
	Vec3 vup = Vec3::cross(u,look).normalized();
	double aspect = double(width) / double(height);
	Camera cam(lookFrom, lookAt, vup, vfov, aspect);

	// Light (directional)
	Light light;
	light.direction = Vec3(-0.7, -1.0, -0.4).normalized();
	light.color = Vec3(1.0, 1.0, 1.0);

	// Materials
	Material red; red.albedo = Vec3(0.9, 0.25, 0.25); red.shininess = 64.0;
	Material green; green.albedo = Vec3(0.25, 0.9, 0.25); green.shininess = 16.0;
	Material gray; gray.albedo = Vec3(0., 0.6, 0.6); gray.shininess = 32.0;

	// Scene objects
	std::vector<std::shared_ptr<Hittable>> objects;

	// Ground as two triangles (a large quad)
	
	// double s = 3.0;
	// Vec3 a(-s, 0.0, -s);
	// Vec3 b(s, 0.0, -s);
	// Vec3 c(s, 0.0, s);
	// Vec3 d(-s, 0.0, s);
	// objects.push_back(std::make_shared<Triangle>(a, b, c, gray));
	// objects.push_back(std::make_shared<Triangle>(a, c, d, gray));

	// Vec3 e(-1, 0.1, 0);
	// Vec3 f(1, 0.1, -1);
	// Vec3 g(1, 0.1, 1);
	// objects.push_back(std::make_shared<Triangle>(e, f, g, green));
	// Sphere
	// objects.push_back(std::make_shared<Sphere>(Vec3(0.0, 0.6, 0.0), 0.6, red));

	// Load OBJ file (using command line parameters)
	if (file_exists(objPath)) {
		MeshLoader::loadOBJ(objPath, scale, translate, green, objects);
		std::cout << "Loaded OBJ: " << objPath << " (scale=" << scale << ", translate=" 
		          << translate.x << "," << translate.y << "," << translate.z << ")\n";
	}
	else {
		std::cout << "OBJ not found (" << objPath << "), continuing without it.\n";
	}
	

	// Toon parameters with a warm ramp
	ToonParams toon;
	toon.diffuseBands = 3;
	toon.silhouetteThreshold = 0.2; // stronger outlines
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
	double depthEdgeThreshold = 0.7; // Increased threshold for Sobel operator to make edges thinner

	if (renderer.renderPPM(objects, toon, outputPath, enableDepthEdges, depthEdgeThreshold)) {
		std::cout << "Wrote: " << outputPath << "\n";
	}
	else {
		std::cerr << "Render failed.\n";
	}
	return 0;
}


