#pragma once
#include <memory>
#include "ray.h"

struct Material;

struct HitRecord {
	double t = 0.0;
	Vec3 point;
	Vec3 normal;
	const Material* material = nullptr;
	bool front_face = true;
	void set_face_normal(const Ray& r, const Vec3& outward_normal) {
		front_face = Vec3::dot(r.direction, outward_normal) < 0.0;
		normal = front_face ? outward_normal : outward_normal * -1.0;
	}
};

class Hittable {
public:
	virtual ~Hittable() = default;
	virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const = 0;
};


