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
    String Attr;
    String Val;
    Pair(const String& attr, const String& val): Attr(attr), Val(val) {}
};
typedef List<Pair> AttributesList;
struct Section {
    Array<String> Selectors;
    AttributesList Attributes;
    Section(const Array<String>& selectors, const AttributesList& attributes): Selectors(selectors), Attributes(attributes) {}
    static bool CompareSelectors(Section& tested, String& target) {
        for(int i = 0; i<tested.Selectors.Length(); i++)
				if (tested.Selectors[i] == target)
					return true;
        return false;
    }
    static bool CompareAttributes(Section& tested, String& target) {
        for (int i = 0; i < tested.Attributes.Count(); i++) {
            if (tested.Attributes[i]->Attr == target)
                return true;
        }
        return false;
    }
    static bool CompareAttributes(Pair& tested, String& target) {
        return tested.Attr == target;
    }
};
typedef List<Section> BlocksStorage;

bool parseStyles(BlocksStorage& blocks) {
    char c;
    ParsingMode ParsingMode = ParsingMode::Selector;
    Array<String> selectors;
    AttributesList attributes;
    String attr;
    String val;
    int Qcount = 0;
    String buffer;
    while (c = getchar()) {
        if (c == EOF)
            return true;
        if (c < ' ')
            continue;
        if (c == ' ' && buffer.Length() == 0)
            continue;
        if (c == '{' && ParsingMode == ParsingMode::Selector) {
            ParsingMode = ParsingMode::Property;
            buffer.RemoveTrailingSpaces();
            if (!selectors.HasElement(buffer))
                if (buffer.Length())
                    selectors.Add(buffer);
            buffer.Clear();
            continue;
        }
        if (c == ',' && ParsingMode == ParsingMode::Selector) {
            buffer.RemoveTrailingSpaces();
            if (!selectors.HasElement(buffer))
                if (buffer.Length())
                    selectors.Add(buffer);
            buffer.Clear();
            continue;
        }
        if (c == '}' && (ParsingMode == ParsingMode::Property || ParsingMode == ParsingMode::Value)) {
            ParsingMode = ParsingMode::Selector;
            blocks.Add(*new Section(selectors,attributes ));
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
            val = buffer;
            buffer.Clear();
            if (attributes.Get(attr, Section::CompareAttributes) == nullptr)
                attributes.Add(*new Pair(attr, val));
            else
                attributes.Get(attr, Section::CompareAttributes)->Val = val;
            continue;
        }
        if (c == '?' && ParsingMode == ParsingMode::Selector) {
            if (++Qcount == 4)
                return false;
            continue;
        }
        Qcount = 0;
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

bool executeCommands(BlocksStorage& blocks) {
    char c;
    ParsingMode ParsingMode = ParsingMode::CommandAttr1;
    String buffer;
    Command cmd;
    while (c = getchar()) {
        if (c == '\n' || c == EOF) {
            if (ParsingMode == ParsingMode::CommandAttr1)
                cmd.param1 = buffer;
            if (cmd.param1.Length() == 0&&c!=EOF)
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
                    printf("%s,S,? == %d\n", cmd.param1.ToValidString(), blocks.Count(cmd.param1,Section::CompareSelectors));
                else {
                    auto target = blocks[block - 1];
                    if (target != nullptr)
                        printf("%d,S,? == %d\n", block, blocks[block - 1]->Selectors.Length());
                }
            }
            else if (cmd.option == 'S') {
                int block = cmd.param1.ToInt();
                int selector = cmd.param2.ToInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    auto name = target->Selectors[selector - 1];
                    if (name.Length())
                        printf("%d,S,%d == %s\n", block, selector, name.ToValidString());
                }
            }
            else if (cmd.option == 'A' && cmd.param2[0] == '?') {
                int block = cmd.param1.ToInt(true);
                if (block == -1) {
                    printf("%s,A,? == %d\n", cmd.param1.ToValidString(), blocks.Count(cmd.param1,Section::CompareAttributes));
                }
                else {
                    auto target = blocks[block - 1];
                    if (target != nullptr)
                        printf("%d,A,? == %d\n", block, target->Attributes.Count());
                }
            }
            else if (cmd.option == 'A') {
                int block = cmd.param1.ToInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    auto value = target->Attributes.Get(cmd.param2,Section::CompareAttributes);
                    if(value!=nullptr)
                        if (value->Val.Length() != 0)
                            printf("%d,A,%s == %s\n", block, cmd.param2.ToValidString(), value->Val.ToValidString());
                }
            }
            else if (cmd.option == 'E') {
                auto target = blocks.Get(cmd.param1,Section::CompareSelectors);
                if (target != nullptr) {
                    auto value = target->Attributes.Get(cmd.param2,Section::CompareAttributes);
                    if(value!=nullptr)
                        if (value->Val.Length())
                            printf("%s,E,%s == %s\n", cmd.param1.ToValidString(), cmd.param2.ToValidString(), value->Val.ToValidString());
                }
            }
            else if (cmd.option == 'D' && cmd.param2[0] == '*') {
                int block = cmd.param1.ToInt();
                if (blocks.Remove(block - 1)!=-1)
                    printf("%d,D,* == deleted\n", block);
            }
            else if (cmd.option == 'D') {
                int block = cmd.param1.ToInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    int elementsLeft = target->Attributes.Remove(cmd.param2,Section::CompareAttributes);
                    if (elementsLeft == 0)
                        blocks.Remove(block - 1);
                    if (elementsLeft != -1)
                        printf("%d,D,%s == deleted\n", block, cmd.param2.ToValidString());
                }
            }
            if (c == EOF)
                return true;
            ParsingMode = ParsingMode::CommandAttr1;
            cmd.option = 'X';
            continue;
        }
        else if (c < 32 && c != EOF)
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
    BlocksStorage blocks;

    while (1) {
        if (parseStyles(blocks))
            break;
        if (executeCommands(blocks))
            break;
    }
    return 0;
}