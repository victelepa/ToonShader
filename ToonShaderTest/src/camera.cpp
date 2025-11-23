#include "camera.h"
#include <cmath>

static inline double degrees_to_radians(double d) { return d * 3.14159265358979323846 / 180.0; }

Camera::Camera() {
	// Default simple camera
	Vec3 lookFrom(0, 1, 3);
	Vec3 lookAt(0, 0.5, 0);
	Vec3 vup(0, 1, 0);
	double vfov = 45.0;
	double aspect = 16.0 / 9.0;

	double theta = degrees_to_radians(vfov);
	double h = std::tan(theta / 2.0);
	double viewport_height = 2.0 * h;
	double viewport_width = aspect * viewport_height;

	w_axis = (lookFrom - lookAt).normalized();
	u_axis = Vec3::cross(vup, w_axis).normalized();
	v_axis = Vec3::cross(w_axis, u_axis);

	origin = lookFrom;
	horizontal = u_axis * viewport_width;
	vertical = v_axis * viewport_height;
	lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - w_axis;
}

Camera::Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up, double verticalFovDegrees, double aspectRatio) {
	double theta = degrees_to_radians(verticalFovDegrees);
	double h = std::tan(theta / 2.0);
	double viewport_height = 2.0 * h;
	double viewport_width = aspectRatio * viewport_height;

	w_axis = (lookFrom - lookAt).normalized();
	u_axis = Vec3::cross(up, w_axis).normalized();
	v_axis = Vec3::cross(w_axis, u_axis);

	origin = lookFrom;
	horizontal = u_axis * viewport_width;
	vertical = v_axis * viewport_height;
	lower_left_corner = origin - horizontal / 2.0 - vertical / 2.0 - w_axis;
}

Ray Camera::get_ray(double u, double v) const {
	// u,v in [0,1] across the viewport
	Vec3 dir = (lower_left_corner + horizontal * u + vertical * v - origin).normalized();
	return Ray(origin, dir);
}


