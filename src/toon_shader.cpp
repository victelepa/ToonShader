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
	
	static inline Vec3 lerp(const Vec3& a, const Vec3& b, double t) {
		return a + (b - a) * t;
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
		return Vec3(0.0, 0.0, 0.0);
	}

	// 光照方向与向量 Lighting vectors
	Vec3 L = (-light.direction).normalized(); //从表面点指向光源的方向（入射光方向） direction from point to light
	Vec3 N = hit.normal; //击中点法线 already oriented against the ray
	Vec3 V = viewDir; //从点指向相机的单位向量

	// // 漫反射项（Lambert），然后量化到色带 Diffuse term (Lambert), quantized into bands for a toon ramp
	// 漫反射项（Lambert），使用ndotl在rampColors中进行lerp插值
	double ndotl = std::max(0.0, std::min(1.0, Vec3::dot(N, L)));
	// int bandIdx = quantize_band(ndotl, params.diffuseBands); // 化得到色带索引
	// 根据 rampColors 和 rampPositions 进行lerp插值
	Vec3 bandColor = Vec3(1.0, 1.0, 1.0);
	size_t numColors = params.rampColors.size();
	
	if (numColors == 1) {
		// 只有一个颜色：使用这个颜色 * ndotl
		bandColor = params.rampColors[0] * ndotl;
	} else {
		// 创建局部副本，在首尾添加元素
		std::vector<Vec3> rampColors = params.rampColors;
		std::vector<double> rampPositions = params.rampPositions;
		rampColors.insert(rampColors.begin(), rampColors[0]); // 首部：第一个颜色
		rampColors.push_back(rampColors[rampColors.size() - 1]); // 尾部：最后一个颜色（注意此时size已经+1）
		// rampColors.insert(rampColors.begin(), Vec3(0.0, 0.0, 0.0));
		// rampColors.push_back(Vec3(1.0, 1.0, 1.0));
		
		rampPositions.insert(rampPositions.begin(), 0.0); // 首部：0
		rampPositions.push_back(1.0); // 尾部：1
		
		// 现在rampColors和rampPositions的大小都是 numColors + 2
		size_t extendedSize = rampColors.size();
		
		// 找到ndotl在哪个区间内（现在ndotl总是在[0,1]范围内，而rampPositions也是[0, ..., 1]）
		int idx0 = 0;
		int idx1 = extendedSize - 1;
		
		for (size_t i = 0; i < extendedSize - 1; i++) {
			if (ndotl >= rampPositions[i] && ndotl <= rampPositions[i + 1]) {
				idx0 = i;
				idx1 = i + 1;
				break;
			}
		}
		
		// 计算插值参数t
		double t = 0.0;
		if (idx0 != idx1) {
			double pos0 = rampPositions[idx0];
			double pos1 = rampPositions[idx1];
			if (pos1 > pos0) {
				t = (ndotl - pos0) / (pos1 - pos0);
			}
		}
		
		// 进行lerp插值
		bandColor = lerp(rampColors[idx0], rampColors[idx1], t);
	}

	// Base toon diffuse: rampColor modulated by material albedo and light color
	// 基础漫反射：色带颜色 * 材质漫反射系数 * 光颜色（按分量相乘）
	// Vec3 baseDiffuse = Vec3::hadamard(Vec3::hadamard(bandColor, hit.material->albedo), light.color);		//直接混色再乘光照
	// Vec3 baseDiffuse = Vec3::hadamard(Vec3::hadamard(bandColor, light.color), hit.material->albedo);		//先乘光照后混色
	Vec3 baseDiffuse = Vec3::hadamard(bandColor, light.color);		//不进行混色，直接放到最后相加


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




	// === 3.5 新增：Rim Light / 边缘光 ===
    Vec3 rimTerm(0.0, 0.0, 0.0);
    if (params.enableRim) {
        // 经典 Fresnel 风格边缘光：视线越贴边缘，值越大
        double ndotv = std::clamp(Vec3::dot(N, V), -1.0, 1.0);
        double rim   = 1.0 - std::fabs(ndotv);   // 中心 0，边缘 1

        // 可选：减去一个阈值，让更靠边才开始出现边缘光
        if (params.rimThreshold > 0.0) {
            rim = std::max(0.0, rim - params.rimThreshold) /
                  std::max(1e-6, 1.0 - params.rimThreshold);
        }

        // “硬度”：类似 pow(·, n)，n 越大越贴边；这就是你们的 Toon 控制
        rim = std::pow(rim, params.rimPower);

        // 强度与颜色
        rimTerm = params.rimColor * (rim * params.rimIntensity);
    }




	// Small ambient to prevent full black in unlit regions (can be tuned or removed)
	// 小环境光（防止未被照亮区域全黑）
	Vec3 ambient = hit.material->albedo;

	// Final color = ambient + toon diffuse + toon specular
	// 最终颜色 = 小环境光 + 基础漫反射 + 高光项
	Vec3 color = ambient + baseDiffuse + specular + rimTerm;
	// 应用输出亮度控制
	color = color * params.outputBrightness;
	
	return Vec3::clamp01(color);
}


