#pragma once
#include "vec3.h"

struct Material {
	Vec3 albedo = Vec3(0.8, 0.8, 0.8);
	Vec3 specularColor = Vec3(1.0, 1.0, 1.0);
	double shininess = 32.0; // Phong exponent
};


