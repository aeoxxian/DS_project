#ifndef BATTLE_STATS_H
#define BATTLE_STATS_H

#include <string>

struct BattleStats {
    std::string label;
    int  damageDealt;
    int  damageTaken;
    int  turns;
    bool isBoss;

    BattleStats()
        : label("?"), damageDealt(0), damageTaken(0), turns(0), isBoss(false) {}
};

#endif
