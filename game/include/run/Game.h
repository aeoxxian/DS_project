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
#include "event/Event.h"
#include "registry/Registries.h"
#include "save/UnlockSave.h"
#include "save/DeckSave.h"

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
    int         charTotalDamage[MAX_CHARACTERS];
    UnlockSave  unlocks;
    DeckSave    deckSave;

    int  showTitleScreen();       // 0=기본덱 / 1=해금덱 / 2=프리셋 로드 완료
    bool tryLoadPreset(int idx); // 슬롯 idx → party+deck 복원. 성공 시 true
    void selectParty();
    void selectStartingDeck();
    void buildDefaultStarterDeck();
    void buildPool();
    void addCardAndUnlock(const Card& c);
    bool pickRandomValidCard(Card& out) const;
    void offerCardReplace(const Card& offered);
    void saveDeckPreset(const std::string& name);
    void handleBattle(bool isBoss = false);
    void handleEvent();
    void handleShop();
    void handleRest();
    void printRunSummary() const;
    void applyOutcome(const EventOutcome& out);
    void removeSelectedCardFromDeck();
    void showRunResult(bool alive) const;

public:
    Game(bool isRetry = false);
    bool run();  // true = 재도전 선택
};

#endif
