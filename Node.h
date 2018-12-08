#ifndef NODE_H
#define NODE_H

#include "Collidable.h"

class Node {
public:
	Node* next;
	Collidable* data;

	Node();

	Node(Node* _next, Collidable* _data);
};

#endif NODE_H