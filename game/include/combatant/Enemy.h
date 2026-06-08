#ifndef ENEMY_H
#define ENEMY_H

#include "combatant/Combatant.h"
#include "registry/MonsterRegistry.h"
#include "ds/Queue.h"

class Enemy : public Combatant {
private:
    EnemySkill        skills[MAX_ENEMY_SKILLS];
    int               skillCount;
    int               patternIndex;
    Queue<EnemySkill> intentQueue;

    EnemySkill rollNextSkill();

public:
    Enemy();
    Enemy(const std::string& name, int maxHP, int atk, int def, Track track);
    Enemy(const EnemyDef& def);

    void prepareIntent();
    bool executeAndQueue(EnemySkill& out);
    bool peekIntent(EnemySkill& out) const;

    int               getSkillCount()     const;
    const EnemySkill& getSkill(int index) const;

    void printStatus() const override;
};

#endif
