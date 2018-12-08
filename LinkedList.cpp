#include "LinkedList.h"
#include "Node.h"
#include <iostream>
//#include <cstdlib>

LinkedList::LinkedList() {
	length = 0;
	head = NULL;
	tail = NULL;
}

void LinkedList::add(Collidable* data) {
	Node* node = new Node(NULL, data);
	if (length == 0) {
		head = node;
		tail = node;
	}
	else {
		tail->next = node;
		tail = node;
	}
	length++;
}

void LinkedList::remove(Collidable* data) {
	Node* current = head;
	Node* prev;
	int i = 0;
	while (current) {
		if (current->data == data) {
			if (i == 0) {
				head = current->next;
				delete current;
			}
			else if (i == length - 1) {
				prev->next = NULL;
				tail = prev;
				delete current;
			}
			else {
				prev->next = current->next;
				delete current;
			}

			length--;

			return;
		}

		prev = current;
		current = current->next;
		i++;
	}
}