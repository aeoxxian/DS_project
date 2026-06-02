#ifndef BATTLE_H
#define BATTLE_H

#include "Constants.h"
#include "BattleCharacter.h"
#include "Enemy.h"
#include "Hand.h"
#include "CardEffect.h"
#include "ds/CardPool.h"

class Battle {
private:
    // 파티 원본 포인터 — Run의 party[] 직접 참조
    BattleCharacter* party[MAX_CHARACTERS];
    int partySize;

    Enemy enemies[MAX_ENEMIES];
    int   enemyCount;

    Hand     hand;
    CardPool& pool;

    int  turnNumber;
    bool battleOver;
    bool playerWon;
    int  pendingDraw;   // 다음 턴 드로우에 더해질 보너스 (Draw 카드가 적립, 1턴 지속)

    // ── Turn phases ───────────────────────────────────────────────
    int  drawCards(int n);   // 풀에서 최대 n장 손패로 (10장 상한). 뽑은 수 반환
    void drawPhase();
    void displayBattleState() const;
    void assignPhase();
    void executePhase();
    void statusTickPhase();
    void endTurnPhase();
    void rewardPhase();

    // ── Effect resolution ─────────────────────────────────────────
    void applyEffect(const CardEffect& effect, BattleCharacter& source, int targetEnemyIdx);

    // ── Damage pipeline ───────────────────────────────────────────
    // 출력 데미지: base + atk_up - atk_down, 그 뒤 weaken% 감소
    int  outgoingDamage(int base, StatusTracker& src) const;
    // 피격 데미지: block -> evade -> shield 흡수 -> def 보정 (방어 상태를 소모/적용)
    int  applyIncoming(int dmg, StatusTracker& tgt) const;
    void dealToEnemy(int enemyIdx, int raw, StatusTracker& src);

    // ── Helpers ───────────────────────────────────────────────────
    bool checkBattleEnd();
    int  firstLivingEnemy() const;
    int  livingPartyCount() const;

public:
    // party[]와 크기를 직접 전달 — 원본 수정됨
    Battle(BattleCharacter party[], int partySize, CardPool& sharedPool);

    void addEnemy(const Enemy& enemy);

    bool run();
    bool isPlayerWon() const;
};

#endif
