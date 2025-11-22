#pragma once
#include "hittable.h"
#include "material.h"

class Triangle : public Hittable {
public:
	Triangle() : v0(), v1(), v2(), face_normal(), material() {}
	Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Material& m);
	bool hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const override;

private:
	Vec3 v0, v1, v2;
	Vec3 face_normal; // constant per face
	Material material;
};


