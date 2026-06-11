#ifndef CARD_POOL_H
#define CARD_POOL_H

#include "card/Card.h"
#include "core/Track.h"
#include "ds/LinkedList.h"

// 파티 공유 카드풀 — 런 전체에서 유지되는 덱
class CardPool {
private:
    LinkedList<Card> cards;

public:
    bool addCard(const Card& card);
    bool getCard(int index, Card& out) const;
    bool removeCard(int index);
    int  size()             const;
    bool isEmpty()          const;
    void clear();
    void print()            const;
    int  countByTrack(Track t) const;
    bool canAddCard(const Card& c) const;
};

// 손패 — 배열 기반 고정 슬롯 (인덱스 안정: 카드 사용 시 슬롯 번호 유지)
class Hand {
private:
    Card slots[MAX_HAND_SIZE];
    bool active[MAX_HAND_SIZE];
    int  slotCount;   // 할당된 슬롯 수 (addCard 시 증가)
    int  activeCount; // 현재 활성(미사용) 카드 수

public:
    Hand();
    bool addCard(const Card& card);
    bool removeCard(int index, Card& out);
    bool restoreCard(int index, const Card& card);  // undo용: 원래 슬롯에 복원
    bool peekCard(int index, Card& out) const;
    int  size()      const;  // 활성 카드 수
    int  slotSize()  const;  // 할당된 슬롯 수 (표시용 반복 범위)
    bool isEmpty()   const;
    void clear();
    void print()     const;
};

#endif
