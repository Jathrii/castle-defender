#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "Model_3DS.h"
#include "Vector3f.h"

class Collidable {
public:
	// Model
	Model_3DS model;

	// Transformations
	Vector3f pos;
	Vector3f rot;
	float scale;

	// Bounds
	float bound_radius;
	float bound_height;

	Collidable();

	Collidable(Model_3DS &_model, Vector3f &_pos, Vector3f &_rot, float _scale, float _bound_radius, float _bound_height);

	bool operator&(Collidable &c);

	void drawBounds();

	void draw();
};

#endif COLLIDABLE_H