#include "List.h"
#include "Array.h"
#include "BlocksStorage.h"
#include <stdio.h>

enum class ParsingMode {
    Selector,
    Property,
    Value,
    CommandAttr1,
    CommandAttr2,
    CommandAttr3,
};

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
        if (c == ' ' && buffer.length() == 0)
            continue;
        if (c == '{' && ParsingMode == ParsingMode::Selector) {
            ParsingMode = ParsingMode::Property;
            buffer.removeTrailingSpaces();
            if (!selectors.hasElement(buffer))
                if (buffer.length())
                    selectors += buffer;
            buffer.clear();
            continue;
        }
        if (c == ',' && ParsingMode == ParsingMode::Selector) {
            buffer.removeTrailingSpaces();
            if (!selectors.hasElement(buffer))
                if (buffer.length())
                    selectors += buffer;
            buffer.clear();
            continue;
        }
        if (c == '}' && (ParsingMode == ParsingMode::Property || ParsingMode == ParsingMode::Value)) {
            ParsingMode = ParsingMode::Selector;
            blocks.Add(selectors, attributes);
            selectors.clear();
            attributes.Clear();
            buffer.clear();
            continue;
        }
        if (c == ':' && ParsingMode == ParsingMode::Property) {
            ParsingMode = ParsingMode::Value;
            attr = buffer;
            buffer.clear();
            continue;
        }
        if (c == ';' && ParsingMode == ParsingMode::Value) {
            ParsingMode = ParsingMode::Property;
            val = buffer;
            buffer.clear();
            attributes.Add(attr, val);
            continue;
        }
        if (c == '?' && ParsingMode == ParsingMode::Selector) {
            if (++Qcount == 4)
                return false;
            continue;
        }
        Qcount = 0;
        buffer += c;
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
            if (cmd.param1.length() == 0&&c!=EOF)
                continue;
            if (ParsingMode == ParsingMode::CommandAttr2)
                cmd.option = buffer[0];
            if (ParsingMode == ParsingMode::CommandAttr3)
                cmd.param2 = buffer;
            buffer.clear();
            if (cmd.param1[0] == '*' && cmd.param1[1] == '*' && cmd.param1[2] == '*' && cmd.param1[3] == '*')
                return false;
            else if (cmd.option == 'X' && (cmd.param1[0] == '?' || cmd.param1[1] == '?')) {
                printf("? == %d\n", blocks.Count());
            }
            else if (cmd.option == 'S' && cmd.param2[0] == '?') {
                int block = cmd.param1.toInt(true);
                if (block == -1)
                    printf("%s,S,? == %d\n", cmd.param1.toValidString(), blocks.Count(cmd.param1));
                else {
                    auto target = blocks[block - 1];
                    if (target != nullptr)
                        printf("%d,S,? == %d\n", block, blocks[block - 1]->Selectors.length());
                }
            }
            else if (cmd.option == 'S') {
                int block = cmd.param1.toInt();
                int selector = cmd.param2.toInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    auto name = target->Selectors[selector - 1];
                    if (name.length())
                        printf("%d,S,%d == %s\n", block, selector, name.toValidString());
                }
            }
            else if (cmd.option == 'A' && cmd.param2[0] == '?') {
                int block = cmd.param1.toInt(true);
                if (block == -1) {
                    printf("%s,A,? == %d\n", cmd.param1.toValidString(), blocks.CountAttribute(cmd.param1));
                }
                else {
                    auto target = blocks[block - 1];
                    if (target != nullptr)
                        printf("%d,A,? == %d\n", block, target->Attributes.Count());
                }
            }
            else if (cmd.option == 'A') {
                int block = cmd.param1.toInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    auto value = target->Attributes.Get(cmd.param2);
                    if (value.length() != 0)
                        printf("%d,A,%s == %s\n", block, cmd.param2.toValidString(), value.toValidString());
                }
            }
            else if (cmd.option == 'E') {
                auto target = blocks.Get(cmd.param1);
                if (target != nullptr) {
                    auto value = target->Attributes.Get(cmd.param2);
                    if (value.length())
                        printf("%s,E,%s == %s\n", cmd.param1.toValidString(), cmd.param2.toValidString(), value.toValidString());
                }
            }
            else if (cmd.option == 'D' && cmd.param2[0] == '*') {
                int block = cmd.param1.toInt();
                if (blocks.Remove(block - 1))
                    printf("%d,D,* == deleted\n", block);
            }
            else if (cmd.option == 'D') {
                int block = cmd.param1.toInt();
                auto target = blocks[block - 1];
                if (target != nullptr) {
                    int elementsLeft = target->Attributes.Remove(cmd.param2);
                    if (elementsLeft == 0)
                        blocks.Remove(block - 1);
                    if (elementsLeft != -1)
                        printf("%d,D,%s == deleted\n", block, cmd.param2.toValidString());
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
                buffer.clear();
                ParsingMode = ParsingMode::CommandAttr2;
            }
            else if (ParsingMode == ParsingMode::CommandAttr2) {
                cmd.option = buffer[0];
                buffer.clear();
                ParsingMode = ParsingMode::CommandAttr3;
            }
            continue;
        }
        buffer += c;
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