#include "Collidable.h"
#include <glut.h>

Collidable::Collidable() {}

Collidable::Collidable(Model_3DS &_model, Vector3f &_pos, Vector3f &_rot, float _scale, float _bound_radius, float _bound_height) {
	model = _model;
	pos = _pos;
	rot = _rot;
	scale = _scale;
	bound_radius = _bound_radius;
	bound_height = _bound_height;
}

bool Collidable::operator&(Collidable &c) {
	float x = pos.x - c.pos.x;
	float y = (pos.y + bound_height * scale) - (c.pos.y + c.bound_height * scale);
	float z = pos.z - c.pos.z;
	double distSquared = x * x + y * y + z * z;
	double radiiSumSquared = (bound_radius * scale + c.bound_radius * c.scale);
	radiiSumSquared *= (bound_radius * scale + c.bound_radius * c.scale);

	if (distSquared <= radiiSumSquared)
		return true;
	else
		return false;
}

void Collidable::drawBounds() {
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(scale, scale, scale);
	glTranslatef(0, bound_height, 0.0);
	glRotatef(rot.y, 0, 1, 0);
	glRotatef(rot.x, 1, 0, 0);
	glRotatef(rot.z, 0, 0, 1);
	glutSolidSphere(bound_radius, 100, 100);
}

void Collidable::draw() {
	glTranslatef(pos.x, pos.y, pos.z);
	glScalef(scale, scale, scale);
	glRotatef(rot.y, 0, 1, 0);
	glRotatef(rot.x, 1, 0, 0);
	glRotatef(rot.z, 0, 0, 1);
	model.Draw();
}