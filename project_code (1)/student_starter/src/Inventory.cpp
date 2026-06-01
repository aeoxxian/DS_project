#include "ds/Inventory.h"
#include <iostream>

Inventory::Inventory() : head(nullptr), count(0) {}

Inventory::~Inventory() {
    clear();
}

void Inventory::addItem(const Item& item) {
    // TODO(Student): Implement linked-list insertion.
    // Suggested behavior: insert the new item at the front of the list.
    // Steps:
    // 1. Allocate a new Node.
    // 2. Point the new node's next to the current head.
    // 3. Update head.
    // 4. Increment count.
    (void)item;
    std::cout << "[TODO] Inventory::addItem is not implemented yet.\n";
}

bool Inventory::removeItem(const std::string& itemName) {
    // TODO(Student): Implement linked-list deletion by item name.
    // Edge cases: empty list, removing the head, removing a middle/tail node,
    // and item not found.
    (void)itemName;
    std::cout << "[TODO] Inventory::removeItem is not implemented yet.\n";
    return false;
}

Item* Inventory::findItem(const std::string& itemName) {
    // TODO(Student): Implement linked-list search.
    (void)itemName;
    return nullptr;
}

const Item* Inventory::findItem(const std::string& itemName) const {
    // TODO(Student): Implement const linked-list search.
    (void)itemName;
    return nullptr;
}

bool Inventory::isEmpty() const {
    return count == 0;
}

int Inventory::size() const {
    return count;
}

void Inventory::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    count = 0;
}

void Inventory::print() const {
    if (head == nullptr) {
        std::cout << "Inventory is empty.\n";
        return;
    }

    std::cout << "Inventory (" << count << " item(s)):\n";
    Node* current = head;
    while (current != nullptr) {
        std::cout << "  - ";
        current->item.print();
        current = current->next;
    }
}
