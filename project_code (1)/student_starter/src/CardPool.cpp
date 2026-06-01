#include "ds/CardPool.h"
#include <iostream>
#include <cstdlib>   // rand

CardPool::CardPool() : poolSize(0) {}

bool CardPool::addCard(const Card& card) {
    if (poolSize >= MAX_POOL_SIZE) return false;
    cards[poolSize++] = card;
    return true;
}

bool CardPool::getCard(int index, Card& out) const {
    if (index < 0 || index >= poolSize) return false;
    out = cards[index];
    return true;
}

bool CardPool::removeCard(int index) {
    if (index < 0 || index >= poolSize) return false;
    for (int i = index; i < poolSize - 1; ++i) {
        cards[i] = cards[i + 1];
    }
    --poolSize;
    return true;
}

int CardPool::size() const { return poolSize; }
bool CardPool::isEmpty() const { return poolSize == 0; }

void CardPool::print() const {
    std::cout << "=== Card Pool (" << poolSize << " cards) ===\n";
    for (int i = 0; i < poolSize; ++i) {
        std::cout << "  " << i << ". ";
        cards[i].print();
        std::cout << "\n";
    }
}
