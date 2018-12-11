#include "Vector3f.h"
#include <cmath>

#define DEG2RAD(a) (a * 0.0174532925)

Vector3f::Vector3f() {
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Vector3f::Vector3f(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

Vector3f Vector3f::operator+ (Vector3f &v) {
	return Vector3f(x + v.x, y + v.y, z + v.z);
}

Vector3f Vector3f::operator-(Vector3f &v) {
	return Vector3f(x - v.x, y - v.y, z - v.z);
}

Vector3f Vector3f::operator*(float n) {
	return Vector3f(x * n, y * n, z * n);
}

Vector3f Vector3f::operator/(float n) {
	return Vector3f(x / n, y / n, z / n);
}

Vector3f Vector3f::unit() {
	return *this / sqrt(x * x + y * y + z * z);
}

Vector3f Vector3f::cross(Vector3f v) {
	return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

Vector3f Vector3f::rotateY(float a) {
	return Vector3f(x * cos(DEG2RAD(a)) + z * sin(DEG2RAD(a)), y, z * cos(DEG2RAD(a)) - x * sin(DEG2RAD(a)));
}
