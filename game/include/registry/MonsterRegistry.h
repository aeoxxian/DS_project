#ifndef MONSTER_REGISTRY_H
#define MONSTER_REGISTRY_H

#include "combatant/CombatantDef.h"
#include "card/Card.h"
#include "core/Constants.h"

struct EnemySkill {
    Card card;
    EnemySkill() {}
    EnemySkill(const Card& c) : card(c) {}
};

struct EnemyDef : public CombatantDef {
    bool       isBoss;
    EnemySkill skills[MAX_ENEMY_SKILLS];
    int        skillCount;

    EnemyDef() : CombatantDef(), isBoss(false), skillCount(0) {}
    EnemyDef(const std::string& name, int hp, int atk, int def,
             int mgc, Track track, bool isBoss = false)
        : CombatantDef(name, hp, atk, def, mgc, track),
          isBoss(isBoss), skillCount(0) {}

    bool addSkill(const Card& c) {
        if (skillCount >= MAX_ENEMY_SKILLS) return false;
        skills[skillCount++] = EnemySkill(c);
        return true;
    }
};

class MonsterRegistry {
private:
    EnemyDef entries[MAX_MONSTER_REGISTRY_SIZE];
    int      count;
    MonsterRegistry();

public:
    static MonsterRegistry& instance();
    bool registerMonster(const EnemyDef& def);
    bool getAt(int index, EnemyDef& out) const;
    bool findByName(const std::string& name, EnemyDef& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllMonsters();

#endif
