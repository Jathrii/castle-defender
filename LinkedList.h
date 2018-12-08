#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"

class LinkedList {
public:
	int length;
	Node* head;
	Node* tail;

	LinkedList();

	void add(Collidable* data);

	void remove(Collidable* data);
};

#endif LINKEDLIST_H