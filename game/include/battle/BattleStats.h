#ifndef BATTLE_STATS_H
#define BATTLE_STATS_H

#include "core/Constants.h"
#include <string>

struct BattleStats {
    std::string label;
    int  damageDealt;
    int  damageTaken;
    int  turns;
    bool isBoss;
    int  charDamage[MAX_CHARACTERS];

    BattleStats()
        : label("?"), damageDealt(0), damageTaken(0), turns(0), isBoss(false) {
        for (int i = 0; i < MAX_CHARACTERS; ++i) charDamage[i] = 0;
    }
};

#endif
