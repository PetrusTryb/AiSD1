#include "BlocksStorage.h"

BlocksStorage::BlocksStorage()
{
	Clear();
}

void BlocksStorage::Add(Array<String>& selectors, AttributesList& attributes)
{
	if (nodes == 0) {
		Node* node = new Node;
		tail = node;
		head = node;
		nodes++;
	}
	if (tail->elements == BLOCKSSTORAGE_BLOCK_SIZE) {
		Node* node = new Node;
		tail->Next = node;
		node->Prev = tail;
		tail = node;
		nodes++;
	}
	tail->Value[tail->elements] = new Block;
	tail->Value[tail->elements]->Selectors = selectors;
	tail->Value[tail->elements]->Attributes = attributes;
	tail->elements++;
	overallElements++;
}

BlocksStorage::Block* BlocksStorage::Get(String& selector)
{
	Node* n = tail;
	while (n != nullptr) {
		for (int i = n->elements - 1; i >= 0; i--) {
			for (int j = 0; j < n->Value[i]->Selectors.length(); j++) {
				if (n->Value[i]->Selectors[j] == selector) {
					return n->Value[i];
				}
			}
		}
		n = n->Prev;
	}
	return nullptr;
}

BlocksStorage::Block* BlocksStorage::operator[](int index)
{
	if (index < 0 || index >= overallElements) {
		return nullptr;
	}
	int i = 0;
	Node* n = head;
	while (n != nullptr) {
		for (int j = 0; j < n->elements; j++) {
			if (i == index) {
				return n->Value[j];
			}
			i++;
		}
		n = n->Next;
	}
	return nullptr;
}

int BlocksStorage::Count(String& selector)
{
	int count = 0;
	Node* n = head;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			for (int j = 0; j < n->Value[i]->Selectors.length(); j++) {
				if (n->Value[i]->Selectors[j] == selector) {
					count++;
				}
			}
		}
		n = n->Next;
	}
	return count;
}

int BlocksStorage::Count()
{
	return overallElements;
}

int BlocksStorage::CountAttribute(String& name)
{
	int count = 0;
	Node* n = head;
	while (n != nullptr) {
		for (int i = 0; i < n->elements; i++) {
			count += n->Value[i]->Attributes.Get(name).length()>0;
		}
		n = n->Next;
	}
	return count;
}

bool BlocksStorage::Remove(int index)
{
	if (index < 0 || index >= overallElements) {
		return false;
	}
	int i = 0;
	Node* n = head;
	while (n != nullptr) {
		for (int j = 0; j < n->elements; j++) {
			if (i == index) {
				for (int k = j; k < n->elements - 1; k++) {
					n->Value[k] = n->Value[k + 1];
				}
				n->elements--;
				overallElements--;
				if (n->elements == 0) {
					if(n->Prev!=nullptr)
						n->Prev->Next = n->Next;
					else
						head = n->Next;
					if(n->Next!=nullptr)
						n->Next->Prev = n->Prev;
					else
						tail = n->Prev;
					delete n;
					nodes--;
				}
				return true;
			}
			i++;
		}
		n = n->Next;
	}
	return false;
}
