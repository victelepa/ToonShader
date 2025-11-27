#include "toon_shader.h"
#include <iostream>

namespace {
	static inline int quantize_band(double value01, int bands) {
		if (bands <= 1) return value01 > 0.0 ? 1 : 0;
		value01 = std::max(0.0, std::min(1.0, value01));
		int band = static_cast<int>(value01 * bands); // [0..bands]
		if (band >= bands) band = bands - 1;
		return band;
	}
}

Vec3 ToonShader::shade(const HitRecord& hit,
	const Vec3& viewDir,
	const Light& light,
	const ToonParams& params) {
	// Silhouette test: when the view is nearly tangent to the surface, we paint pure black.
	// This creates a crisp outline independent of lighting.
	double nv = std::fabs(Vec3::dot(hit.normal, viewDir));
	if (nv < params.silhouetteThreshold) {
		return Vec3(0.9, 0.2, 0.9);
	}

	// Lighting vectors
	Vec3 L = (-light.direction).normalized(); // direction from point to light
	Vec3 N = hit.normal; // already oriented against the ray
	Vec3 V = viewDir;

	// Diffuse term (Lambert), quantized into bands for a toon ramp
	double ndotl = std::max(0.0, Vec3::dot(N, L));
	int bandIdx = quantize_band(ndotl, params.diffuseBands);

	Vec3 bandColor = Vec3(1.0, 1.0, 1.0);
	if (!params.rampColors.empty() && bandIdx < (int)params.rampColors.size()) {
		bandColor = params.rampColors[bandIdx];
	}
	else {
		// fallback: grayscale ramp
		double t = (params.diffuseBands <= 1) ? ndotl : (double)(bandIdx + 1) / (double)(params.diffuseBands);
		bandColor = Vec3(t, t, t);
	}

	// Base toon diffuse: rampColor modulated by material albedo and light color
	Vec3 baseDiffuse = Vec3::hadamard(Vec3::hadamard(bandColor, hit.material->albedo), light.color);

	// Hard-edge specular from Phong term: compute, then threshold into bands.
	Vec3 R = Vec3::reflect(-L, N); // reflect incoming light about the normal
	double rdotv = std::max(0.0, Vec3::dot(R, V));
	double phong = std::pow(rdotv, std::max(1.0, hit.material->shininess));

	Vec3 specular(0, 0, 0);
	if (phong > params.specularThreshold1) {
		specular = Vec3::hadamard(params.specColorA, hit.material->specularColor);
	}
	else if (phong > params.specularThreshold2) {
		specular = Vec3::hadamard(params.specColorB, hit.material->specularColor);
	}
	// else 0

	// Small ambient to prevent full black in unlit regions (can be tuned or removed)
	Vec3 ambient = hit.material->albedo * 0.05;

	Vec3 color = ambient + baseDiffuse + specular;
	return Vec3::clamp01(color);
}


