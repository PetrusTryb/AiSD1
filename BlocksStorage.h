#pragma once
#include "List.h"
#include "AttributesList.h"
#include "Array.h"
class BlocksStorage : public List
{
protected:
	struct Block {
		Array<String> Selectors;
		AttributesList Attributes;
	};
	struct Node
	{
		Block* Value[LIST_BLOCK_SIZE] = {};
		Node* Next = nullptr;
		Node* Prev = nullptr;
		int elements = 0;
	};
	Node* head = nullptr;
	Node* tail = nullptr;
public:
	void Add(Array<String>& selectors, AttributesList& attributes);
	Block* Get(String& selector);
	Block* operator[](int index);
	int CountSelector(String& selector);
	int CountAttribute(String& name);
	bool Remove(int index);
};

