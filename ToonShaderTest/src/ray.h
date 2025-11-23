#pragma once
#include "vec3.h"

struct Ray {
	Vec3 origin;
	Vec3 direction; // should be normalized
	Ray() : origin(), direction(0, 0, 1) {}
	Ray(const Vec3& o, const Vec3& d) : origin(o), direction(d) {}
	Vec3 at(double t) const { return origin + direction * t; }
};


