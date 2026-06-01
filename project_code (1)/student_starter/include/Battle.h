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

    // ── Turn phases ───────────────────────────────────────────────
    void drawPhase();
    void displayBattleState() const;
    void assignPhase();
    void executePhase();
    void statusTickPhase();
    void endTurnPhase();
    void rewardPhase();

    // ── Effect resolution ─────────────────────────────────────────
    void applyEffect(const CardEffect& effect, BattleCharacter& source, int targetEnemyIdx);

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
