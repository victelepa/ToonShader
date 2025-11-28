#pragma once
#include "vec3.h"

/// @brief 材质结构体
struct Material {
	/// @brief 漫反射颜色
	Vec3 albedo = Vec3(0.8, 0.8, 0.8);

	/// @brief 镜面反射颜色
	Vec3 specularColor = Vec3(1.0, 1.0, 1.0);

	/// @brief 高光强度(Phong指数)
	double shininess = 32.0;
};


