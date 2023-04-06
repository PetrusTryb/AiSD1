#include "List.h"
#include "Array.h"
#include <stdio.h>

enum class ParsingMode {
	Selector,
	Property,
	Value,
	CommandAttr1,
	CommandAttr2,
	CommandAttr3,
};

struct Pair {
	String attr;
	String val;
	Pair() {}
	Pair(const String& attr, const String& val) : attr(attr), val(val) {}
};
typedef List<String> SelectorsList;
typedef List<Pair> AttributesList;
struct Section {
	SelectorsList selectors;
	AttributesList attributes;
	Section() {}
	Section(const SelectorsList& selectors, const AttributesList& attributes) : selectors(selectors), attributes(attributes) {}
	static bool CompareSelectors(Section& tested, String& target) {
		return tested.selectors.HasElement(target, CompareSelector);
	}
	static bool CompareSelector(String& tested, String& target) {
		return tested == target;
	}
	static bool CompareAttributes(Section& tested, String& target) {
		return tested.attributes.HasElement(target, CompareAttribute);
	}
	static bool CompareAttribute(Pair& tested, String& target) {
		return tested.attr == target;
	}
};
typedef List<Section> BlocksList;

bool parseStyles(BlocksList& blocks) {
	char c = 0;
	ParsingMode ParsingMode = ParsingMode::Selector;
	SelectorsList selectors;
	AttributesList attributes;
	String attr;
	String buffer;
	while (c != EOF) {
		c = getchar();
		if (c < ' ' || (c == ' ' && buffer.Length() == 0))
			continue;
		if (c == '{' && ParsingMode == ParsingMode::Selector) {
			ParsingMode = ParsingMode::Property;
			buffer.RemoveTrailingSpaces();
			if (!selectors.HasElement(buffer, Section::CompareSelector) && buffer.Length())
				selectors.Add(buffer);
			buffer.Clear();
			continue;
		}
		if (c == ',' && ParsingMode == ParsingMode::Selector) {
			buffer.RemoveTrailingSpaces();
			if (!selectors.HasElement(buffer, Section::CompareSelector) && buffer.Length())
				selectors.Add(buffer);
			buffer.Clear();
			continue;
		}
		if (c == '}' && (ParsingMode == ParsingMode::Property || ParsingMode == ParsingMode::Value)) {
			ParsingMode = ParsingMode::Selector;
			blocks.Add({ selectors, attributes });
			selectors.Clear();
			attributes.Clear();
			buffer.Clear();
			continue;
		}
		if (c == ':' && ParsingMode == ParsingMode::Property) {
			ParsingMode = ParsingMode::Value;
			attr = buffer;
			buffer.Clear();
			continue;
		}
		if (c == ';' && ParsingMode == ParsingMode::Value) {
			ParsingMode = ParsingMode::Property;
			if (attributes.Get(attr, Section::CompareAttribute) == nullptr)
				attributes.Add({ attr, buffer });
			else
				attributes.Get(attr, Section::CompareAttribute)->val = buffer;
			buffer.Clear();
			continue;
		}
		if (c == '?' && buffer[0] == '?' && buffer[1] == '?' && buffer[2] == '?' && ParsingMode == ParsingMode::Selector) {
			return false;
		}
		buffer.Add(c);
	}
	return true;
}

struct Command
{
	String param1;
	char option = 'X';
	String param2;
};

bool executeCommands(BlocksList& blocks) {
	char c = 0;
	ParsingMode ParsingMode = ParsingMode::CommandAttr1;
	String buffer;
	Command cmd;
	while (c != EOF) {
		c = getchar();
		if (c == '\n' || c == EOF) {
			if (ParsingMode == ParsingMode::CommandAttr1)
				cmd.param1 = buffer;
			if (cmd.param1.Length() == 0)
				continue;
			if (ParsingMode == ParsingMode::CommandAttr2)
				cmd.option = buffer[0];
			if (ParsingMode == ParsingMode::CommandAttr3)
				cmd.param2 = buffer;
			buffer.Clear();
			if (cmd.param1[0] == '*' && cmd.param1[1] == '*' && cmd.param1[2] == '*' && cmd.param1[3] == '*')
				return false;
			else if (cmd.option == 'X' && (cmd.param1[0] == '?' || cmd.param1[1] == '?')) {
				printf("? == %d\n", blocks.Count());
			}
			else if (cmd.option == 'S' && cmd.param2[0] == '?') {
				int block = cmd.param1.ToInt(true);
				if (block == -1)
					printf("%s,S,? == %d\n", cmd.param1.ToValidString(), blocks.Count(cmd.param1, Section::CompareSelectors));
				else {
					auto target = blocks[block - 1];
					if (target != nullptr)
						printf("%d,S,? == %d\n", block, blocks[block - 1]->selectors.Count());
				}
			}
			else if (cmd.option == 'S') {
				int block = cmd.param1.ToInt();
				int selector = cmd.param2.ToInt();
				auto target = blocks[block - 1];
				if (target != nullptr) {
					auto name = target->selectors[selector - 1];
					if (name != nullptr)
						printf("%d,S,%d == %s\n", block, selector, name->ToValidString());
				}
			}
			else if (cmd.option == 'A' && cmd.param2[0] == '?') {
				int block = cmd.param1.ToInt(true);
				if (block == -1) {
					printf("%s,A,? == %d\n", cmd.param1.ToValidString(), blocks.Count(cmd.param1, Section::CompareAttributes));
				}
				else {
					auto target = blocks[block - 1];
					if (target != nullptr)
						printf("%d,A,? == %d\n", block, target->attributes.Count());
				}
			}
			else if (cmd.option == 'A') {
				int block = cmd.param1.ToInt();
				auto target = blocks[block - 1];
				if (target != nullptr) {
					auto value = target->attributes.Get(cmd.param2, Section::CompareAttribute);
					if (value != nullptr)
						if (value->val.Length() != 0)
							printf("%d,A,%s == %s\n", block, cmd.param2.ToValidString(), value->val.ToValidString());
				}
			}
			else if (cmd.option == 'E') {
				auto target = blocks.Get(cmd.param1, Section::CompareSelectors);
				if (target != nullptr) {
					auto value = target->attributes.Get(cmd.param2, Section::CompareAttribute);
					if (value != nullptr)
						if (value->val.Length())
							printf("%s,E,%s == %s\n", cmd.param1.ToValidString(), cmd.param2.ToValidString(), value->val.ToValidString());
				}
			}
			else if (cmd.option == 'D' && cmd.param2[0] == '*') {
				int block = cmd.param1.ToInt();
				if (blocks.Remove(block - 1) != -1)
					printf("%d,D,* == deleted\n", block);
			}
			else if (cmd.option == 'D') {
				int block = cmd.param1.ToInt();
				auto target = blocks[block - 1];
				if (target != nullptr) {
					int elementsLeft = target->attributes.Remove(cmd.param2, Section::CompareAttribute);
					if (elementsLeft == 0)
						blocks.Remove(block - 1);
					if (elementsLeft != -1)
						printf("%d,D,%s == deleted\n", block, cmd.param2.ToValidString());
				}
			}
			ParsingMode = ParsingMode::CommandAttr1;
			cmd.option = 'X';
			continue;
		}
		else if (c < ' ')
			continue;
		if (c == ',') {
			if (ParsingMode == ParsingMode::CommandAttr1) {
				cmd.param1 = buffer;
				buffer.Clear();
				ParsingMode = ParsingMode::CommandAttr2;
			}
			else if (ParsingMode == ParsingMode::CommandAttr2) {
				cmd.option = buffer[0];
				buffer.Clear();
				ParsingMode = ParsingMode::CommandAttr3;
			}
			continue;
		}
		buffer.Add(c);
	}
	return true;
}

int main()
{
	BlocksList blocks;

	while (1) {
		if (parseStyles(blocks))
			break;
		if (executeCommands(blocks))
			break;
	}
	return 0;
}