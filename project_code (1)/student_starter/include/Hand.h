#ifndef HAND_H
#define HAND_H

#include "Constants.h"
#include "Card.h"

class Hand {
private:
    Card cards[MAX_HAND_SIZE];
    int count;

public:
    Hand();

    bool addCard(const Card& card);
    bool removeCard(int index, Card& out);
    bool peekCard(int index, Card& out) const;

    int  size() const;
    bool isEmpty() const;
    bool isFull() const;
    void clear();
    void print() const;
};

#endif
