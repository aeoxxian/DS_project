#ifndef INVENTORY_H
#define INVENTORY_H

#include "run/Item.h"
#include "ds/LinkedList.h"

class Inventory {
private:
    LinkedList<Item> items;

public:
    void addItem(const Item& item);
    bool removeItem(const std::string& name, Item& out);
    bool findItem(const std::string& name, Item& out) const;
    int  size()    const;
    bool isEmpty() const;
    void print()   const;
};

#endif
