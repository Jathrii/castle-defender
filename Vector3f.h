#ifndef VECTOR3F_H
#define VECTOR3F_H

class Vector3f {
public:
	float x, y, z;

	Vector3f();

	Vector3f(float _x, float _y, float _z);

	Vector3f operator+(Vector3f &v);

	Vector3f operator-(Vector3f &v);

	Vector3f operator*(float n);

	Vector3f operator/(float n);

	Vector3f unit();

	Vector3f cross(Vector3f v);

	Vector3f rotateY(float a);
};

#endif VECTOR3F_H