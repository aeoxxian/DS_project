#ifndef COMBATANT_DEF_H
#define COMBATANT_DEF_H

#include "core/Track.h"
#include <string>

struct CombatantDef {
    std::string name;
    int baseHP, baseAttack, baseDefend;
    Track track;

    CombatantDef() : baseHP(0), baseAttack(0), baseDefend(0), track(Track::None) {}
    CombatantDef(const std::string& n, int hp, int atk, int def, Track t)
        : name(n), baseHP(hp), baseAttack(atk), baseDefend(def), track(t) {}
};

#endif
