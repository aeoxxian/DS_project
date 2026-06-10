#ifndef BATTLE_H
#define BATTLE_H

#include "core/Constants.h"
#include "combatant/BattleCharacter.h"
#include "combatant/Enemy.h"
#include "effect/Effect.h"
#include "card/CardPool.h"
#include "battle/BattleStats.h"
#include "ds/Stack.h"
#include "run/Inventory.h"

class Battle {
private:
    struct AssignRecord { int ci; Card card; };

    BattleCharacter* party[MAX_CHARACTERS];
    int      partySize;
    Enemy    enemies[MAX_ENEMIES];
    int      enemyCount;
    Hand     hand;
    CardPool&  pool;
    Inventory* inv;
    int      turnNumber;
    bool     battleOver;
    bool     playerWon;
    int      totalDamageDealt;
    int      totalDamageTaken;
    int      pendingDrawBonus;
    int      charDamageDealt[MAX_CHARACTERS];
    int      currentActorIdx;
    Stack<AssignRecord> assignStack;

    void drawPhase();
    void displayBattleState() const;
    void printHand() const;
    void assignPhase();
    void executePhase();
    void statusTickPhase();
    void endTurnPhase();
    void rewardPhase();

    void applyEffect(const Effect& e, Combatant& source, int targetIdx, bool sourceIsPlayer);
    void resolveCard(const Card& card, Combatant& source, bool sourceIsPlayer, int targetIdx);

    // 공격 헬퍼
    int  frontPartyIndex()                          const;
    int  calcRaw(const Effect& e, Combatant& src)   const;
    void dealToEnemy(int ei, int raw, EffectType t);
    void dealToAllEnemies(int raw, EffectType t);
    void dealToParty(int raw, EffectType t);

    bool checkBattleEnd();
    int  firstLivingEnemy() const;
    int  livingPartyCount() const;

public:
    Battle(BattleCharacter party[], int partySize, CardPool& pool, Inventory* inv = nullptr);
    void addEnemy(const Enemy& enemy);
    bool        run();
    bool        isPlayerWon() const;
    BattleStats getStats()    const;
};

#endif
