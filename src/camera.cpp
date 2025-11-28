#include "camera.h"
#include <cmath>
#include <iostream>

/// @brief 将角度转换为弧度
/// @param d 角度值
/// @return 弧度值
static inline double degrees_to_radians(double d) { return d * 3.14159265358979323846 / 180.0; }


Camera::Camera() {
	/// @brief 相机的世界坐标位置
	Vec3 lookFrom(0, 1, 3);

	/// @brief 相机指向的目标位置
	Vec3 lookAt(0, 0.5, 0);

	/// @brief 相机的上方向
	Vec3 vup(0, 1, 0);

	/// @brief 垂直视野角度（单位：度）
	double vfov = 45.0;

	/// @brief 屏幕宽高比
	double aspect = 16.0 / 9.0;

	/// @brief 将角度转换为弧度
	double theta = degrees_to_radians(vfov);

	/// @brief 计算视口高度和宽度
	double h = std::tan(theta / 2.0);
	double viewport_height = 2.0 * h;
	double viewport_width = aspect * viewport_height;

	//相机坐标系
	/// @brief 相机到场景的方向向量
	w_axis = (lookAt - lookFrom).normalized();  // Fixed: direction from camera to scene

	/// @brief 相机的右方向向量
	u_axis = Vec3::cross(w_axis, vup).normalized();  // Fixed: right direction (swapped order to fix left-right flip)

	/// @brief 相机的上方向向量
	v_axis = Vec3::cross(w_axis, u_axis);

	origin = lookFrom;
	horizontal = u_axis * viewport_width;
	vertical = v_axis * viewport_height;
	lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 + w_axis;  // Fixed: viewport should be in front of camera
}


Camera::Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up, double verticalFovDegrees, double aspectRatio) {
	double theta = degrees_to_radians(verticalFovDegrees);
	double h = std::tan(theta / 2.0);
	double focal_length = 1;  // Distance from camera to viewport
	double viewport_height = 2.0 * h * focal_length;
	double viewport_width = aspectRatio * viewport_height;

	w_axis = (lookAt - lookFrom).normalized();  // Fixed: direction from camera to scene
	u_axis = Vec3::cross(w_axis, up).normalized();  // Fixed: right direction (swapped order to fix left-right flip)
	// v_axis = Vec3::cross(w_axis, u_axis);
	v_axis = up;

	origin = lookFrom;
	horizontal = u_axis * viewport_width;
	vertical = v_axis * viewport_height;
	lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 + w_axis * focal_length;  // Viewport at focal_length distance
	
	// Debug output
	std::cout << "\n[Camera Debug] ========== Camera Setup ==========\n";
	std::cout << "lookFrom: (" << lookFrom.x << ", " << lookFrom.y << ", " << lookFrom.z << ")\n";
	std::cout << "lookAt: (" << lookAt.x << ", " << lookAt.y << ", " << lookAt.z << ")\n";
	std::cout << "up: (" << up.x << ", " << up.y << ", " << up.z << ")\n";
	std::cout << "w_axis: (" << w_axis.x << ", " << w_axis.y << ", " << w_axis.z << ")\n";
	std::cout << "u_axis: (" << u_axis.x << ", " << u_axis.y << ", " << u_axis.z << ")\n";
	std::cout << "v_axis: (" << v_axis.x << ", " << v_axis.y << ", " << v_axis.z << ")\n";
	std::cout << "lower_left_corner: (" << lower_left_corner.x << ", " << lower_left_corner.y << ", " << lower_left_corner.z << ")\n";
	std::cout << "viewport_width: " << viewport_width << ", viewport_height: " << viewport_height << "\n";
	std::cout << "[Camera Debug] ===================================\n\n";
}

Ray Camera::get_ray(double u, double v) const {
	// u,v in [0,1] across the viewport
	Vec3 dir = (lower_left_corner + horizontal * u + vertical * v - origin).normalized();
	return Ray(origin, dir);
}


