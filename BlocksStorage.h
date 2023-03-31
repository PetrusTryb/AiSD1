#pragma once
#include "List.h"
#include "AttributesList.h"
#include "Array.h"
#define BLOCKSSTORAGE_BLOCK_SIZE 8
class BlocksStorage : public List
{
protected:
	struct Block {
		Array<String> Selectors;
		AttributesList Attributes;
	};
	struct Node
	{
		Block* Value[BLOCKSSTORAGE_BLOCK_SIZE] = {};
		Node* Next = nullptr;
		Node* Prev = nullptr;
		int elements = 0;
	};
	Node* head = nullptr;
	Node* tail = nullptr;
public:
	BlocksStorage();
	void Add(Array<String>& selectors, AttributesList& attributes);
	Block* Get(String& selector);
	Block* operator[](int index);
	int Count(String& selector);
	int Count();
	int CountAttribute(String& name);
	bool Remove(int index);
};

