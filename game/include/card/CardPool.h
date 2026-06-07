#ifndef CARD_POOL_H
#define CARD_POOL_H

#include "card/Card.h"
#include "ds/LinkedList.h"

// 파티 공유 카드풀 — 런 전체에서 유지되는 덱
class CardPool {
private:
    LinkedList<Card> cards;

public:
    bool addCard(const Card& card);
    bool getCard(int index, Card& out) const;
    bool removeCard(int index);
    int  size()    const;
    bool isEmpty() const;
    void print()   const;
};

// 손패 — 매 턴 드로우/사용으로 변경되는 임시 카드 묶음
class Hand {
private:
    LinkedList<Card> cards;

public:
    bool addCard(const Card& card);
    bool removeCard(int index, Card& out);
    bool peekCard(int index, Card& out) const;
    int  size()    const;
    bool isEmpty() const;
    void clear();
    void print()   const;
};

#endif
