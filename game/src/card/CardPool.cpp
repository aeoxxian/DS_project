#include "card/CardPool.h"
#include <iostream>

// ── CardPool ──────────────────────────────────────────────────────────────────

bool CardPool::addCard(const Card& card) { cards.pushBack(card); return true; }

bool CardPool::getCard(int index, Card& out) const { return cards.getAt(index, out); }

bool CardPool::removeCard(int index) {
    Card tmp;
    return cards.removeAt(index, tmp);
}

int  CardPool::size()    const { return cards.size(); }
bool CardPool::isEmpty() const { return cards.isEmpty(); }

void CardPool::print() const {
    std::cout << "=== Card Pool (" << cards.size() << " cards) ===\n";
    auto* node = cards.getHead();
    int i = 0;
    while (node) {
        std::cout << "  " << i++ << ". "; node->value.print(); std::cout << "\n";
        node = node->next;
    }
}

// ── Hand ──────────────────────────────────────────────────────────────────────

bool Hand::addCard(const Card& card) { cards.pushBack(card); return true; }

bool Hand::removeCard(int index, Card& out) { return cards.removeAt(index, out); }

bool Hand::peekCard(int index, Card& out) const { return cards.getAt(index, out); }

int  Hand::size()    const { return cards.size(); }
bool Hand::isEmpty() const { return cards.isEmpty(); }
void Hand::clear()         { cards.clear(); }

void Hand::print() const {
    std::cout << "=== 손패 (" << cards.size() << "장) ===\n";
    auto* node = cards.getHead();
    int i = 0;
    while (node) {
        std::cout << "  " << i++ << ". "; node->value.print(); std::cout << "\n";
        node = node->next;
    }
}
