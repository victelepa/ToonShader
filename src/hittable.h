#pragma once
#include <memory>
#include "ray.h"

struct Material;

/// @brief 击中记录结构体
struct HitRecord {
	/// @brief 击中时间
	double t = 0.0;
	/// @brief 击中点
	Vec3 point;
	/// @brief 击中点的法线
	Vec3 normal;
	/// @brief 击中点的材质
	const Material* material = nullptr;
	/// @brief 击中点是否为正面
	bool front_face = true;

	/// @brief 设置击中点的法线
	/// @param r 入射光线
	/// @param outward_normal 出射法线
	void set_face_normal(const Ray& r, const Vec3& outward_normal) 
	{
		front_face = Vec3::dot(r.direction, outward_normal) < 0.0;
		normal = front_face ? outward_normal : outward_normal * -1.0;
	}
};

/// @brief 可击中对象基类
class Hittable {
public:
	virtual ~Hittable() = default;

	/// @brief 判断光线是否击中对象
	/// @param r 射线
	/// @param t_min 最小击中时间
	/// @param t_max 最大击中时间
	/// @param out_rec 击中记录
	/// @return 是否击中对象
	virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const = 0;
};


