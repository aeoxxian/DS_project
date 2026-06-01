#ifndef CARD_POOL_H
#define CARD_POOL_H

#include "Card.h"
#include "Constants.h"

class CardPool {
private:
    Card cards[MAX_POOL_SIZE];
    int poolSize;

public:
    CardPool();

    bool addCard(const Card& card);
    bool getCard(int index, Card& out) const;
    bool removeCard(int index);

    int  size() const;
    bool isEmpty() const;
    void print() const;
};

#endif
