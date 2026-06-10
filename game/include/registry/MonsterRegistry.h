#ifndef MONSTER_REGISTRY_H
#define MONSTER_REGISTRY_H

#include "combatant/CombatantDef.h"
#include "card/Card.h"
#include "core/Constants.h"

struct EnemyDef : public CombatantDef {
    bool isBoss;
    Card cards[MAX_ENEMY_SKILLS];
    int  cardCount;

    EnemyDef() : CombatantDef(), isBoss(false), cardCount(0) {}
    EnemyDef(const std::string& name, int hp, int atk, int def,
             Track track, bool isBoss = false)
        : CombatantDef(name, hp, atk, def, track),
          isBoss(isBoss), cardCount(0) {}

    bool addCard(const Card& c) {
        if (cardCount >= MAX_ENEMY_SKILLS) return false;
        cards[cardCount++] = c;
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
