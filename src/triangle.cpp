#include "triangle.h"

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c, const Material& m)
	: v0(a), v1(b), v2(c), material(m) {
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;
	face_normal = Vec3::cross(e1, e2).normalized();
}

bool Triangle::hit(const Ray& r, double t_min, double t_max, HitRecord& out_rec) const {
	// Moller-Trumbore
	const double EPS = 1e-8;
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;
	Vec3 pvec = Vec3::cross(r.direction, e2);
	double det = Vec3::dot(e1, pvec);
	if (std::fabs(det) < EPS) return false;
	double invDet = 1.0 / det;

	Vec3 tvec = r.origin - v0;
	double u = Vec3::dot(tvec, pvec) / invDet;
	if (u < 0.0 || u > 1.0) return false;

	Vec3 qvec = Vec3::cross(tvec, e1);
	double v = Vec3::dot(r.direction, qvec) / invDet;
	if (v < 0.0 || u + v > 1.0) return false;

	double t = Vec3::dot(e2, qvec) / invDet;
	if (t < t_min || t > t_max) return false;

	out_rec.t = t;
	out_rec.point = r.at(t);
	out_rec.set_face_normal(r, face_normal);
	out_rec.material = &material;
	return true;
}


