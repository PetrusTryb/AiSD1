#pragma once
#include <stdio.h>
class List
{
protected:
	const int PACK = 8;
	struct Node
	{
		void* Value[8] = {};
		Node* Next = nullptr;
		Node* Prev = nullptr;
		int elements = 0;
	};
	Node* head;
	Node* tail;
	int nodes;
	int overallElements;
public:
	List() {
		head = nullptr;
		tail = nullptr;
		nodes = 0;
		overallElements = 0;
	}
	~List() {
		Clear();
	}
	void Clear() {
		Node* n = head;
		if(n != nullptr)
			while (n->Next != nullptr) {
				Node* temp = n;
				n = n->Next;
				delete temp;
			}
		delete n;
		head = nullptr;
		tail = nullptr;
		nodes = 0;
		overallElements = 0;
	}
	int Count() {
		return overallElements;
	}
};

