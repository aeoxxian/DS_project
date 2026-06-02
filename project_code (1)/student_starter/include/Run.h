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
    void buildStartingPool();  // 초기 카드풀 구성 (공용 5장 + 트랙당 2장)
    // 레지스트리에서 조건에 맞는 카드 중 count장을 무작위로 골라 풀에 추가.
    // normalOnly=true면 공용 카드, false면 track 트랙 카드.
    void addRandomCards(bool normalOnly, Track track, int count);

    // ── 상태 출력 ─────────────────────────────────────────────────
    void printPartyStatus() const;

public:
    Run();

    // 런 시작. 클리어 여부 반환.
    bool start();
};

#endif
