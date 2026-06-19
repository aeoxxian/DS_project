#include "run/Inventory.h"
#include "ds/Sorting.h"
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

    // sortItemsByValueDescending(Sorting.h)으로 정렬; 원본 LinkedList 인덱스를 병행 복원
    Item* arr     = new Item[n];
    int*  origIdx = new int[n];
    for (int i = 0; i < n; ++i) {
        items.getAt(i, arr[i]);
        origIdx[i] = i;
    }
    sortItemsByValueDescending(arr, n);   // ds/Sorting.h 선택 정렬
    // 정렬 후 각 위치의 원본 인덱스 복원 (중복 이름·값 처리)
    bool used[MAX_POTIONS] = {};
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!used[j]) {
                Item it; items.getAt(j, it);
                if (it.getName() == arr[i].getName() && it.getValue() == arr[i].getValue()) {
                    origIdx[i] = j;
                    used[j]    = true;
                    break;
                }
            }
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
