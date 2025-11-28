#pragma once
#include "vec3.h"

/// @brief 射线射线方向（单位向量）
struct Ray {
	/// @brief 射线起点
	Vec3 origin;
	/// @brief 射线方向（单位向量）
	Vec3 direction; // should be normalized


	Ray() : origin(), direction(0, 0, 1) {}

	/// @brief 构造函数
	/// @param o 射线起点
	/// @param d 射线方向（单位向量）
	Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
	
	/// @brief 计算射线在参数t上的点
	/// @param t 参数t（通常为距离）
	/// @return 射线在参数t上的点坐标
	Vec3 at(double t) const { return origin + direction * t; }
};


