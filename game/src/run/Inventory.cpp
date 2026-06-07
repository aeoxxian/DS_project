#include "run/Inventory.h"
#include <iostream>

void Inventory::addItem(const Item& item) {
    items.pushBack(item);
}

bool Inventory::removeItem(const std::string& name, Item& out) {
    auto* node = items.getHead();
    int i = 0;
    while (node) {
        if (node->value.getName() == name)
            return items.removeAt(i, out);
        node = node->next;
        ++i;
    }
    return false;
}

bool Inventory::findItem(const std::string& name, Item& out) const {
    auto* node = items.getHead();
    while (node) {
        if (node->value.getName() == name) {
            out = node->value;
            return true;
        }
        node = node->next;
    }
    return false;
}

int  Inventory::size()    const { return items.size(); }
bool Inventory::isEmpty() const { return items.isEmpty(); }

void Inventory::print() const {
    if (items.isEmpty()) {
        std::cout << "  (인벤토리 비어있음)\n";
        return;
    }
    std::cout << "  === 인벤토리 (" << items.size() << "개) ===\n";
    auto* node = items.getHead();
    int i = 0;
    while (node) {
        std::cout << "  [" << i++ << "] ";
        node->value.print();
        node = node->next;
    }
}
