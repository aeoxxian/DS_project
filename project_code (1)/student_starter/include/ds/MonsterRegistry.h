#ifndef MONSTER_REGISTRY_H
#define MONSTER_REGISTRY_H

#include "Enemy.h"
#include "Constants.h"

class MonsterRegistry {
private:
    Enemy entries[MAX_MONSTER_REGISTRY_SIZE];
    int   count;
    MonsterRegistry();

public:
    static MonsterRegistry& instance();

    bool registerMonster(const Enemy& enemy);
    bool getAt(int index, Enemy& out) const;
    bool findByName(const std::string& name, Enemy& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllMonsters();

#endif
