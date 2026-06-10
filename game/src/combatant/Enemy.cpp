#include "combatant/Enemy.h"
#include <iostream>
#include <cstdlib>

Enemy::Enemy() : Combatant(), cardCount(0), patternIndex(0) {}

Enemy::Enemy(const std::string& name, int maxHP, int atk,
             int def, Track track)
    : Combatant(name, maxHP, atk, def, track), cardCount(0), patternIndex(0) {
    Card defaultAtk(0, "공격", "기본 공격", Track::None, TargetScope::Single);
    defaultAtk.addEffect(Effect(EffectType::Attack, atk));
    cards[cardCount++] = defaultAtk;
}

Enemy::Enemy(const EnemyDef& def)
    : Combatant(def), cardCount(def.cardCount), patternIndex(0) {
    for (int i = 0; i < def.cardCount; ++i) cards[i] = def.cards[i];
    if (cardCount == 0) {
        Card defaultAtk(0, "공격", "기본 공격", Track::None, TargetScope::Single);
        defaultAtk.addEffect(Effect(EffectType::Attack, def.baseAttack));
        cards[cardCount++] = defaultAtk;
    }
}

Card Enemy::rollNextCard() {
    if (cardCount == 0) return Card();
    Card next = cards[patternIndex % cardCount];
    patternIndex = (patternIndex + 1) % cardCount;
    return next;
}

void Enemy::prepareIntent() {
    intentQueue.clear();
    intentQueue.enqueue(rollNextCard());
}

bool Enemy::executeAndQueue(Card& out) {
    if (!intentQueue.dequeue(out)) return false;
    intentQueue.enqueue(rollNextCard());
    return true;
}

bool Enemy::peekIntent(Card& out) const {
    return intentQueue.peek(out);
}

void Enemy::printStatus() const {
    Combatant::printStatus();
    Card next;
    if (peekIntent(next))
        std::cout << "    Next: [" << next.getName() << "]\n";
    else
        std::cout << "    Next: ???\n";
}
