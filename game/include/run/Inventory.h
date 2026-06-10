#ifndef INVENTORY_H
#define INVENTORY_H

#include "run/Item.h"
#include "ds/LinkedList.h"
#include "core/Constants.h"

class Inventory {
private:
    LinkedList<Item> items;

public:
    bool addItem(const Item& item);  // 꽉 차면 false 반환
    bool removeItem(const std::string& name, Item& out);
    bool takeAt(int index, Item& out);
    bool findItem(const std::string& name, Item& out) const;
    int  size()    const;
    bool isEmpty() const;
    void print()   const;
};

#endif
