#include "run/Inventory.h"
#include "ds/Sorting.h"
#include <iostream>

bool Inventory::addItem(const Item& item) {
    if (items.size() >= MAX_POTIONS) return false;
    items.pushBack(item);
    return true;
}

bool Inventory::takeAt(int index, Item& out) {
    return items.removeAt(index, out);
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
    int n = items.size();
    if (n == 0) {
        std::cout << "  (인벤토리 비어있음)\n";
        return;
    }

    // LinkedList → 배열 변환 후 가치순 정렬
    Item* arr = new Item[n];
    auto* node = items.getHead();
    for (int i = 0; i < n; ++i, node = node->next) arr[i] = node->value;
    sortItemsByValueDescending(arr, n);

    std::cout << "  === 인벤토리 (" << n << "개, 가치순) ===\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "  [" << i << "] ";
        arr[i].print();
    }
    delete[] arr;
}
