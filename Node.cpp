#include "Node.h"

Node::Node() {}

Node::Node(Node* _next, Collidable* _data) {
	next = _next;
	data = _data;
}