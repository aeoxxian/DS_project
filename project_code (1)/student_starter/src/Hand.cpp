#include "Hand.h"
#include <iostream>

Hand::Hand() : count(0) {}

bool Hand::addCard(const Card& card) {
    if (count >= MAX_HAND_SIZE) return false;
    cards[count++] = card;
    return true;
}

bool Hand::removeCard(int index, Card& out) {
    if (index < 0 || index >= count) return false;
    out = cards[index];
    // shift left
    for (int i = index; i < count - 1; ++i) {
        cards[i] = cards[i + 1];
    }
    --count;
    return true;
}

bool Hand::peekCard(int index, Card& out) const {
    if (index < 0 || index >= count) return false;
    out = cards[index];
    return true;
}

int Hand::size() const { return count; }
bool Hand::isEmpty() const { return count == 0; }
bool Hand::isFull() const { return count >= MAX_HAND_SIZE; }

void Hand::clear() { count = 0; }

void Hand::print() const {
    std::cout << "=== Hand (" << count << "/" << MAX_HAND_SIZE << ") ===\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  " << i << ". ";
        cards[i].print();
        std::cout << "\n";
    }
}
