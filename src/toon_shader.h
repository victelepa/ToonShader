#pragma once
#include <vector>
#include "vec3.h"
#include "hittable.h"
#include "material.h"

struct Light {
	Vec3 direction = Vec3(-0.5, -1.0, -0.2); // direction from light toward the scene
	Vec3 color = Vec3(1.0, 1.0, 1.0);
};

struct ToonParams {
	int diffuseBands = 3; // quantize diffuse into N bands
	double silhouetteThreshold = 0.2; // |dot(N,V)| below this -> pure black outline

	// Hard-edge specular thresholds, t1 > t2 produces two-band specular, else single band
	double specularThreshold1 = 0.6;
	double specularThreshold2 = 0.3;
	Vec3 specColorA = Vec3(1.0, 1.0, 1.0); // color for strong highlight
	Vec3 specColorB = Vec3(0.7, 0.7, 0.7); // color for secondary band

	// Toon ramp colors (size should be >= diffuseBands). If empty, will use grayscale bands.
	std::vector<Vec3> rampColors;
};

namespace ToonShader {
	// Computes a toon-shaded color. Uses:
	//  - Diffuse quantization into bands (color ramp).
	//  - Hard-edge specular: thresholds applied to Phong term.
	//  - Silhouette: if |dot(N,V)| < threshold -> black.
	Vec3 shade(const HitRecord& hit,
		const Vec3& viewDir,      // normalized direction from point to camera
		const Light& light,
		const ToonParams& params);
}


