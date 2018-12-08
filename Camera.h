#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3f.h"

class Camera {
public:
	Vector3f eye, center, up;

	Camera();

	Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

	void moveX(float d);

	void moveY(float d);

	void moveZ(float d);

	void rotateX(float a);

	void rotateY(float a);

	void look();
};

#endif CAMERA_H