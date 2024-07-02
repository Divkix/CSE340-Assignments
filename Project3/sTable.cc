#include "sTable.h"
#include <iostream>
#include <cstring>
#include "lexer.h"

static std::string keyword[] = {"int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

sTable *symbolTable = nullptr;

void addList(std::string name, int line, int type)
{
    if (symbolTable == NULL)
    {
        sTable *newEntry = new sTable();
        sTableEntry *newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNO = line;
        newItem->type = type;
        newItem->isPrinted = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = NULL;

        symbolTable = newEntry;
    }
    else
    {
        sTable *temp = symbolTable;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }

        sTable *newEntry = new sTable();
        sTableEntry *newItem = new sTableEntry();

        newItem->name = name;
        newItem->lineNO = line;
        newItem->type = type;
        newItem->isPrinted = 0;

        newEntry->item = newItem;
        newEntry->next = NULL;
        newEntry->prev = temp;
        temp->next = newEntry;
    }
}

int searchList(std::string n)
{
    sTable *temp = symbolTable;
    bool found = false;

    if (symbolTable == NULL)
    {
        addList(n, 0, 4); // Modify the line number and type as required
        return 4;
    }
    else
    {
        while (temp->next != NULL)
        {
            if (temp->item->name == n)
            {
                found = true;
                return temp->item->type;
            }
            else
            {
                temp = temp->next;
            }
        }
        if (temp->item->name == n)
        {
            found = true;
            return temp->item->type;
        }
        if (!found)
        {
            addList(n, 0, 4); // Modify the line number and type as required
            return 4;
        }
    }
    return 0;
}

void printList(void)
{
    sTable *temp = symbolTable;
    int temp1;
    std::string output = "";

    while (temp->next != NULL)
    {
        if (temp->item->type > 3 && temp->item->isPrinted == 0)
        {
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->isPrinted = 1;

            sTable *lookAhead = temp->next;

            while (temp->next != NULL)
            {
                temp = temp->next;
                if (temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->isPrinted = 1;
                }
            }
            output += ": ? #";
            std::cout << output << std::endl;
            temp->item->isPrinted = 1;
            output = "";
            temp = symbolTable;
        }
        else if (temp->item->type < 4 && temp->item->isPrinted == 0)
        {
            std::string lCase = keyword[(temp->item->type) - 1];
            int temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            std::cout << output << std::endl;
            output = "";
            temp->item->isPrinted = 1;

            while (temp->next != NULL && temp->next->item->type == temp1)
            {
                temp = temp->next;
                std::string lCase2 = keyword[(temp->item->type) - 1];
                output = temp->item->name + ": " + lCase2 + " #";
                std::cout << output << std::endl;
                temp->item->isPrinted = 1;
                output = "";
            }
        }
        else
        {
            temp = temp->next;
        }
    }

    if (temp->item->type <= 3 && temp->item->isPrinted == 0)
    {
        std::string lCase3 = keyword[(temp->item->type) - 1];
        output += temp->item->name + ": " + lCase3 + " #";
        std::cout << output << std::endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->isPrinted == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        std::cout << output << std::endl;
        output = "";
    }
}

void typeUpdating(int currentType, int newType)
{
    sTable *temp = symbolTable;

    while (temp->next != NULL)
    {
        if (temp->item->type == currentType)
        {
            temp->item->type = newType;
        }
        temp = temp->next;
    }
    if (temp->item->type == currentType)
    {
        temp->item->type = newType;
    }
}
