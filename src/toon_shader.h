#pragma once
#include <vector>
#include "vec3.h"
#include "hittable.h"
#include "material.h"

/// @brief 光源结构体
struct Light {
	/// @brief 光线方向（从光源指向场景）
	Vec3 direction = Vec3(-0.5, -1.0, -0.2); 
	/// @brief 光线颜色
	Vec3 color = Vec3(1.0, 1.0, 1.0);
};

/// @brief 卡通渲染参数结构体
struct ToonParams {
	int diffuseBands = 3; // 漫反射分段数量  quantize diffuse into N bands
	double silhouetteThreshold = 0.2; //轮廓判定阈值：|dot(N,V)| < threshold 时视为轮廓/黑色   |dot(N,V)| below this -> pure black outline

	// Hard-edge specular thresholds, t1 > t2 produces two-band specular, else single band
	double specularThreshold1 = 0.6; // 第一层高光阈值（更强的高光）
	double specularThreshold2 = 0.3; // 第二层高光阈值（较弱的高光）
	Vec3 specColorA = Vec3(1.0, 1.0, 1.0); // 强高光颜色 color for strong highlight
	Vec3 specColorB = Vec3(0.7, 0.7, 0.7); // 次级高光颜色 color for secondary band

	// Toon ramp colors (size should be >= diffuseBands). Default: grayscale ramp from black to white.
	// // Toon ramp colors (size should be >= diffuseBands). If empty, will use grayscale bands.
	// std::vector<Vec3> rampColors; // 漫反射色带的颜色表（从暗到亮）。如果为空，着色器应退回到灰度色带。
	std::vector<Vec3> rampColors = {
		Vec3(0.0, 0.0, 0.0),  // 黑色 (最暗)
		Vec3(0.5, 0.5, 0.5),  // 灰色 (中间)
		Vec3(1.0, 1.0, 1.0)   // 白色 (最亮)
	}; // 漫反射色带的颜色表（从暗到亮），默认值为灰度渐变
	
	// Ramp positions: 每个颜色对应的位置值（范围[0,1]），从暗到亮
	// 如果为空或大小不匹配rampColors，将使用均匀分布（0, 1/(n-1), 2/(n-1), ..., 1）
	std::vector<double> rampPositions; // 颜色位置表，与rampColors一一对应
	
	// 最终颜色输出控制：对ambient、baseDiffuse、specular的缩放因子
	double outputBrightness = 1.0; // 输出亮度，可以降低整体亮度（例如0.5表示减半）


	// Rim lighting parameters 边缘光参数
	bool   enableRim     = true;                 // 是否开启边缘光
    Vec3   rimColor      = Vec3(1.0, 1.0, 1.0);  // 边缘光颜色
    double rimIntensity  = 0.5;                  // 强度整体缩放
    double rimPower      = 2.0;                  // 指数：越大越“贴边”才亮
    double rimThreshold  = 0.0;

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


