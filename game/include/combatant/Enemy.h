#ifndef ENEMY_H
#define ENEMY_H

#include "combatant/Combatant.h"
#include "registry/MonsterRegistry.h"
#include "ds/Queue.h"

class Enemy : public Combatant {
private:
    Card       cards[MAX_ENEMY_SKILLS];
    int        cardCount;
    int        patternIndex;
    bool       boss;
    Queue<Card> intentQueue;

    Card rollNextCard();

public:
    Enemy();
    Enemy(const std::string& name, int maxHP, int atk, int def, Track track);
    Enemy(const EnemyDef& def);

    bool isBoss() const { return boss; }

    void prepareIntent();
    bool executeAndQueue(Card& out);
    bool peekIntent(Card& out) const;

    void printStatus() const override;
};

#endif
