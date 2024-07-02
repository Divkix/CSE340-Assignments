#ifndef STABLE_H
#define STABLE_H

#include <string>

struct sTableEntry
{
    std::string name;
    int lineNO;
    int type;
    int isPrinted;
};

struct sTable
{
    sTableEntry *item;
    sTable *prev;
    sTable *next;
};

void addList(std::string name, int line, int type);
int searchList(std::string n);
void printList();
void typeUpdating(int currentType, int newType);

extern sTable *symbolTable;

#endif // STABLE_H
