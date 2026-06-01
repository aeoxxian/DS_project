#ifndef RUN_H
#define RUN_H

#include "Constants.h"
#include "RunMap.h"
#include "ds/EventRegistry.h"
#include "BattleCharacter.h"
#include "Battle.h"
#include "RestRoom.h"
#include "ds/CardPool.h"
#include "ds/MonsterRegistry.h"

// 런 하나의 전체 흐름을 관리.
// 캐릭터 선택 → 맵 이동 → 방 처리(전투/이벤트/휴식/보스) → 런 종료
class Run {
private:
    BattleCharacter party[MAX_CHARACTERS];
    CardPool        pool;
    RunMap          map;
    bool            alive;

    // ── 방 처리 ───────────────────────────────────────────────────
    void handleBattle(bool isBoss = false);
    void handleEvent();
    void handleRest();

    // ── 초기 설정 ─────────────────────────────────────────────────
    void selectCharacters();   // 로스터에서 3명 선택
    void buildStartingPool();  // 초기 카드풀 구성

    // ── 상태 출력 ─────────────────────────────────────────────────
    void printPartyStatus() const;

public:
    Run();

    // 런 시작. 클리어 여부 반환.
    bool start();
};

#endif
