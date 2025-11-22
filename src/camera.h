#pragma once
#include "vec3.h"
#include "ray.h"

class Camera {
public:
	Camera();
	Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up, double verticalFovDegrees, double aspectRatio);

	Ray get_ray(double u, double v) const; // u,v in [0,1]

private:
	Vec3 origin;
	Vec3 lower_left_corner;
	Vec3 horizontal;
	Vec3 vertical;
	Vec3 u_axis, v_axis, w_axis;
};


