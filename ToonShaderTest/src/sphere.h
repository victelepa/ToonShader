#pragma once
#include "hittable.h"
#include "material.h"

class Sphere : public Hittable {
public:
	Sphere() : center(), radius(1.0), material() {}
	Sphere(const Vec3& c, double r, const Material& m) : center(c), radius(r), material(m) {}

	bool hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const override;

private:
	Vec3 center;
	double radius;
	Material material;
};


