#pragma once
#include "vec3.h"
#include "ray.h"

// 相机类
class Camera {
public:
	//默认相机
	Camera();

	/// @brief 相机
	/// @param lookFrom 相机位置
	/// @param lookAt 相机目标位置
	/// @param up 相机向上方向
	/// @param verticalFovDegrees 垂直视野角度
	/// @param aspectRatio 屏幕宽高比
	Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up, double verticalFovDegrees, double aspectRatio);

	/// @brief 生成一条从相机出发经过视口(u,v)的光线
	/// @param u 视口水平坐标 X，范围[0,1]
	/// @param v 视口垂直坐标 Y，范围[0,1]
	Ray get_ray(double u, double v) const; // u,v in [0,1]

private:
	/// @brief 相机位置
	Vec3 origin;

	/// @brief 视口左下角位置
	Vec3 lower_left_corner;
	/// @brief 视口水平向量
	Vec3 horizontal;
	/// @brief 视口垂直向量
	Vec3 vertical;
	/// @brief 相机水平方向向量
	Vec3 u_axis;
	/// @brief 相机垂直方向向量
	Vec3 v_axis;
	/// @brief 相机指向目标方向向量
	Vec3 w_axis;
};


