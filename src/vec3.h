// Minimal 3D vector for CPU shading and ray tracing
#pragma once
#include <cmath>
#include <algorithm>

struct Vec3 {
	double x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}

	Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator-() const { return Vec3(-x, -y, -z); }
	Vec3 operator*(double s) const { return Vec3(x * s, y * s, z * s); }
	Vec3 operator/(double s) const { return Vec3(x / s, y / s, z / s); }
	Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	Vec3& operator*=(double s) { x *= s; y *= s; z *= s; return *this; }
	Vec3& operator/=(double s) { x /= s; y /= s; z /= s; return *this; }

	static Vec3 hadamard(const Vec3& a, const Vec3& b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }

	double length() const { return std::sqrt(x * x + y * y + z * z); }
	double length_squared() const { return x * x + y * y + z * z; }
	Vec3 normalized() const {
		double len = length();
		if (len == 0.0) return Vec3(0, 0, 0);
		return *this / len;
	}

	static double dot(const Vec3& a, const Vec3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	static Vec3 cross(const Vec3& a, const Vec3& b) {
		return Vec3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	static Vec3 reflect(const Vec3& v, const Vec3& n) {
		// Reflect v about normal n (assumes n normalized)
		return v - n * (2.0 * Vec3::dot(v, n));
	}

	static Vec3 clamp01(const Vec3& c) {
		return Vec3(
			std::max(0.0, std::min(1.0, c.x)),
			std::max(0.0, std::min(1.0, c.y)),
			std::max(0.0, std::min(1.0, c.z))
		);
	}
};

inline Vec3 operator*(double s, const Vec3& v) { return v * s; }


