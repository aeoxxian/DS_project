#ifndef REST_ROOM_H
#define REST_ROOM_H

#include "BattleCharacter.h"
#include "ds/CardPool.h"
#include "Constants.h"

// 휴식 방 — HP 회복 or 카드 강화 선택
class RestRoom {
public:
    // characters 배열과 크기, 공유 풀을 받아 처리
    void enter(BattleCharacter characters[], int count, CardPool& pool);

private:
    void healOption(BattleCharacter characters[], int count);
    void upgradeOption(CardPool& pool);
};

#endif
