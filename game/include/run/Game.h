#ifndef GAME_H
#define GAME_H

#include "core/Constants.h"
#include "core/Track.h"
#include "combatant/BattleCharacter.h"
#include "card/CardPool.h"
#include "map/RunMap.h"
#include "battle/Battle.h"
#include "battle/BattleStats.h"
#include "ds/ScoreTree.h"
#include "run/Inventory.h"
#include "registry/Registries.h"

class Game {
private:
    BattleCharacter party[MAX_CHARACTERS];
    CardPool        pool;
    RunMap          map;
    bool            alive;

    Inventory   inventory;
    BattleStats battleLog[MAX_MAP_NODES];
    int         battleLogCount;
    ScoreTree   scoreTree;

    void selectParty();
    void buildPool();
    void handleBattle(bool isBoss = false);
    void handleEvent();
    void handleRest();
    void printRunSummary() const;

public:
    Game();
    void run();
};

#endif
