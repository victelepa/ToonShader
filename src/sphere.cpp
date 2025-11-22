#include "sphere.h"
#include <cmath>

bool Sphere::hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const {
	Vec3 oc = r.origin - center;
	double a = r.direction.length_squared();
	double half_b = Vec3::dot(oc, r.direction);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant < 0.0) return false;
	double sqrtd = std::sqrt(discriminant);

	// Find the nearest root in the acceptable range
	double root = (-half_b - sqrtd) / a;
	if (root < t_min || root > t_max) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || root > t_max) return false;
	}

	out_rec.t = root;
	out_rec.point = r.at(out_rec.t);
	Vec3 outward = (out_rec.point - center) / radius;
	out_rec.set_face_normal(r, outward.normalized());
	out_rec.material = &material;
	return true;
}


