#include "run/Inventory.h"
#include "core/UI.h"
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
    for (int i = 0; i < items.size(); ++i) {
        Item it;
        if (items.getAt(i, it) && it.getName() == name)
            return items.removeAt(i, out);
    }
    return false;
}

bool Inventory::findItem(const std::string& name, Item& out) const {
    for (int i = 0; i < items.size(); ++i) {
        Item it;
        if (items.getAt(i, it) && it.getName() == name) {
            out = it;
            return true;
        }
    }
    return false;
}

int  Inventory::size()    const { return items.size(); }
bool Inventory::isEmpty() const { return items.isEmpty(); }

void Inventory::print() const {
    static const int W = 56;
    int n = items.size();
    std::cout << "\n";
    UI::boxTop(W);
    if (n == 0) {
        UI::boxCenter("인벤토리  (비어있음)", W);
        UI::boxBot(W);
        std::cout << "\n";
        return;
    }

    // 정렬 시 원본 LinkedList 인덱스를 함께 추적 (선택 정렬)
    Item* arr     = new Item[n];
    int*  origIdx = new int[n];
    for (int i = 0; i < n; ++i) {
        items.getAt(i, arr[i]);
        origIdx[i] = i;
    }
    for (int i = 0; i < n - 1; ++i) {
        int maxJ = i;
        for (int j = i + 1; j < n; ++j)
            if (arr[j].getValue() > arr[maxJ].getValue()) maxJ = j;
        if (maxJ != i) {
            Item tmp = arr[i]; arr[i] = arr[maxJ]; arr[maxJ] = tmp;
            int  ti  = origIdx[i]; origIdx[i] = origIdx[maxJ]; origIdx[maxJ] = ti;
        }
    }

    UI::boxCenter("인벤토리  (" + std::to_string(n) + "/" + std::to_string(MAX_POTIONS) + ")", W);
    UI::boxDiv(W);
    for (int i = 0; i < n; ++i) {
        std::string name = arr[i].getName();
        int namePad = std::max(0, 14 - UI::displayWidth(name));
        UI::boxLeft("  [" + std::to_string(origIdx[i]) + "]  " + name
                    + std::string(namePad, ' ') + "  " + arr[i].getDescription(), W);
    }
    UI::boxBot(W);
    std::cout << "\n";
    delete[] origIdx;
    delete[] arr;
}
