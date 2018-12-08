#include "Vector3f.h"
#include "Camera.h"
#include <glut.h>
#include <cmath>

#define DEG2RAD(a) (a * 0.0174532925)

Camera::Camera() {
	eye = Vector3f(1.0f, 1.0f, 1.0f);
	center = Vector3f(0.0f, 0.0f, 0.0f);
	up = Vector3f(0.0f, 1.0f, 0.0f);
}

Camera::Camera(float eyeX, float eyeY, float eyeZ,
	float centerX, float centerY, float centerZ,
	float upX, float upY, float upZ) {
	eye = Vector3f(eyeX, eyeY, eyeZ);
	center = Vector3f(centerX, centerY, centerZ);
	up = Vector3f(upX, upY, upZ);
}

void Camera::moveX(float d) {
	Vector3f right = up.cross(center - eye).unit();
	eye = eye + right * d;
	center = center + right * d;
}

void Camera::moveY(float d) {
	eye = eye + up.unit() * d;
	center = center + up.unit() * d;
}

void Camera::moveZ(float d) {
	Vector3f view = (center - eye).unit();
	eye = eye + view * d;
	center = center + view * d;
}

void Camera::rotateX(float a) {
	Vector3f view = (center - eye).unit();
	Vector3f right = up.cross(view).unit();
	view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
	//up = view.cross(right);
	center = eye + view;
}

void Camera::rotateY(float a) {
	Vector3f view = (center - eye).unit();
	Vector3f right = up.cross(view).unit();
	view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
	right = view.cross(up);
	center = eye + view;
}

void Camera::look() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		eye.x, eye.y, eye.z,
		center.x, center.y, center.z,
		up.x, up.y, up.z
	);
}
