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
void CardPool::clear()         { cards.clear(); }

int CardPool::countByTrack(Track t) const {
    int n = 0;
    for (int i = 0; i < cards.size(); ++i) {
        Card c;
        if (cards.getAt(i, c) && c.getTrack() == t) ++n;
    }
    return n;
}

bool CardPool::canAddCard(const Card& c) const {
    if (c.getTrack() == Track::None)
        return countByTrack(Track::None) < DECK_COMMON_LIMIT;
    return countByTrack(c.getTrack()) < DECK_TRACK_LIMIT;
}

void CardPool::print() const {
    std::cout << "  덱 (" << cards.size() << "장):\n";
    for (int i = 0; i < cards.size(); ++i) {
        Card c;
        if (!cards.getAt(i, c)) continue;
        std::string trackStr = (c.getTrack() == Track::None)
                               ? "[공용]"
                               : "[" + trackToString(c.getTrack()) + "]";
        std::cout << "  " << i << ". " << c.getName()
                  << "  " << trackStr
                  << "  " << c.getDescription() << "\n";
    }
}

// ── Hand (배열 기반, 슬롯 인덱스 안정) ──────────────────────────────────────

Hand::Hand() : slotCount(0), activeCount(0) {
    for (int i = 0; i < MAX_HAND_SIZE; ++i) active[i] = false;
}

bool Hand::addCard(const Card& card) {
    if (slotCount >= MAX_HAND_SIZE) return false;
    slots[slotCount] = card;
    active[slotCount] = true;
    ++slotCount;
    ++activeCount;
    return true;
}

bool Hand::removeCard(int index, Card& out) {
    if (index < 0 || index >= slotCount || !active[index]) return false;
    out = slots[index];
    active[index] = false;
    --activeCount;
    return true;
}

bool Hand::restoreCard(int index, const Card& card) {
    if (index < 0 || index >= slotCount) return false;
    slots[index] = card;
    active[index] = true;
    ++activeCount;
    return true;
}

bool Hand::peekCard(int index, Card& out) const {
    if (index < 0 || index >= slotCount || !active[index]) return false;
    out = slots[index];
    return true;
}

int  Hand::size()      const { return activeCount; }
int  Hand::slotSize()  const { return slotCount; }
bool Hand::isEmpty()   const { return activeCount == 0; }

void Hand::clear() {
    for (int i = 0; i < slotCount; ++i) active[i] = false;
    slotCount   = 0;
    activeCount = 0;
}

void Hand::print() const {
    std::cout << "  Hand (" << activeCount << " cards):\n";
    for (int i = 0; i < slotCount; ++i) {
        if (!active[i]) continue;
        std::cout << "  " << i << ". ";
        slots[i].print();
        std::cout << "\n";
    }
}
