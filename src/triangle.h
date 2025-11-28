#pragma once
#include "hittable.h"
#include "material.h"

class Triangle : public Hittable {
public:
	/// @brief 三角形构造函数
	/// @param a 三角形的第一个顶点
	/// @param b 三角形的第二个顶点
	/// @param c 三角形的第三个顶点
	/// @param m 三角形的材质
	Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Material& m);

	/// @brief 判断光线是否击中三角形
	/// @param r 射线
	/// @param t_min 最小击中时间
	/// @param t_max 最大击中时间
	/// @param out_rec 击中记录
	bool hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const override;

private:

	/// @brief 三角形的三个顶点
	Vec3 v0, v1, v2;

	/// @brief 三角形的面法线（恒定）
	Vec3 face_normal;

	/// @brief 三角形的材质
	Material material;
};


