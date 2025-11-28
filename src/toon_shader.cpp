#include "toon_shader.h"
#include <iostream>

namespace {
	/// @brief 量化漫反射值到色带索引
	/// @param value01 漫反射值，范围[0,1]
	/// @param bands 色带数量
	/// @return 量化后的色带索引，范围[0,bands-1]
	static inline int quantize_band(double value01, int bands) {
		if (bands <= 1) return value01 > 0.0 ? 1 : 0; // 单一色带特殊处理
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

	// 轮廓检测（Silhouette）：
    // 通过 |dot(N, V)| 判断视线相对于表面的切向程度，
    // 当视线几乎与表面切线平行时（值小）认为是轮廓，绘制为纯色（这里使用亮紫色作为示例）。
    // 如果希望黑色轮廓，可改为 Vec3(0,0,0)。

	// 计算视线与法线的点积，判断是否为轮廓
	// 当 |dot(N, V)| 接近于0时，视线与法线垂直，认为是轮廓
	double nv = std::fabs(Vec3::dot(hit.normal, viewDir));
	if (nv < params.silhouetteThreshold) {
		return Vec3(0.9, 0.2, 0.9);
	}

	// 光照方向与向量 Lighting vectors

	Vec3 L = (-light.direction).normalized(); //从表面点指向光源的方向（入射光方向） direction from point to light
	Vec3 N = hit.normal; //击中点法线 already oriented against the ray
	Vec3 V = viewDir; //从点指向相机的单位向量

	// 漫反射项（Lambert），然后量化到色带 Diffuse term (Lambert), quantized into bands for a toon ramp
	double ndotl = std::max(0.0, Vec3::dot(N, L)); // N·L，表示被光照亮的程度
	int bandIdx = quantize_band(ndotl, params.diffuseBands); // 化得到色带索引

	// 根据 rampColors 或灰度回退获得该色带的颜色
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
	// 基础漫反射：色带颜色 * 材质漫反射系数 * 光颜色（按分量相乘）
	Vec3 baseDiffuse = Vec3::hadamard(Vec3::hadamard(bandColor, hit.material->albedo), light.color);

	// Hard-edge specular from Phong term: compute, then threshold into bands.
	// 高光项（Phong），计算后量化到色带 
	Vec3 R = Vec3::reflect(-L, N); //反射向量 reflect incoming light about the normal
	double rdotv = std::max(0.0, Vec3::dot(R, V)); // R·V ，视线与反射光的夹角余弦
	double phong = std::pow(rdotv, std::max(1.0, hit.material->shininess));

	//根据阈值决定高光色带（硬边）：phong > t1 -> 强高光；phong > t2 -> 次高光；否则无高光
	Vec3 specular(0, 0, 0);
	if (phong > params.specularThreshold1) {
		specular = Vec3::hadamard(params.specColorA, hit.material->specularColor);
	}
	else if (phong > params.specularThreshold2) {
		specular = Vec3::hadamard(params.specColorB, hit.material->specularColor);
	}
	// else 0

	// Small ambient to prevent full black in unlit regions (can be tuned or removed)
	// 小环境光（防止未被照亮区域全黑）
	Vec3 ambient = hit.material->albedo * 0.05;

	// Final color = ambient + toon diffuse + toon specular
	// 最终颜色 = 小环境光 + 基础漫反射 + 高光项
	Vec3 color = ambient + baseDiffuse + specular;
	return Vec3::clamp01(color);
}


