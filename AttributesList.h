#pragma once
#include "List.h"
#include "Array.h"
class AttributesList: public List
{
protected:
	struct Pair {
		String Attr;
		String Val;
	};
	struct Node
	{
		Pair* Value[LIST_BLOCK_SIZE] = {};
		Node* Next = nullptr;
		Node* Prev = nullptr;
		int elements = 0;
	};
	Node* head=nullptr;
	Node* tail=nullptr;
public:
	AttributesList(const AttributesList& list);
	AttributesList operator=(const AttributesList& list);
	void Add(String& attribute, String& value);
	String Get(String& name);
	int Remove(String& name);
};

