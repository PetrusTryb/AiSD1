#include "AttributesList.h"

AttributesList::AttributesList()
{
	head = nullptr;
	tail = nullptr;
	nodes = 0;
	overallElements = 0;
}

AttributesList::AttributesList(const AttributesList& list) {
	head = nullptr;
	tail = nullptr;
	nodes = 0;
	overallElements = 0;
	Node* n = list.head;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			Add(n->Value[i]->Attr, n->Value[i]->Val);
		}
		n = n->Next;
	}
}

AttributesList AttributesList::operator=(const AttributesList& list) {
	Clear();
	Node* n = list.head;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			Add(n->Value[i]->Attr, n->Value[i]->Val);
		}
		n = n->Next;
	}
	return *this;
}

void AttributesList::Add(String& attribute, String& value) {
	if (nodes == 0) {
		Node* n = new Node;
		n->Value[0] = new Pair{ attribute, value };
		n->Prev = nullptr;
		n->Next = nullptr;
		n->elements = 1;
		head = n;
		tail = n;
		nodes++;
		overallElements++;
		return;
	}
	Node* n = head;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			if (n->Value[i]->Attr == attribute) {
				n->Value[i]->Val = value;
				return;
			}
		}
		n = n->Next;
	}
	if (tail->elements == PACK) {
		Node* n = new Node;
		n->Value[0] = new Pair{ attribute, value };
		n->Prev = tail;
		n->Next = nullptr;
		n->elements = 1;
		tail->Next = n;
		tail = n;
		nodes++;
		overallElements++;
		return;
	}
	tail->Value[tail->elements] = new Pair{ attribute, value };
	tail->elements++;
	overallElements++;
}

String AttributesList::Get(String& name) {
	Node* n = tail;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			if (n->Value[i]->Attr == name) {
				return n->Value[i]->Val;
			}
		}
		n = n->Prev;
	}
	return String();
}

int AttributesList::Remove(String& name) {
	Node* n = tail;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			if (n->Value[i]->Attr == name) {
				for (int j = i; j < n->elements - 1; j++) {
					n->Value[j] = n->Value[j + 1];
				}
				n->elements--;
				overallElements--;
				return overallElements;
			}
		}
		n = n->Prev;
	}
	return -1;
}
