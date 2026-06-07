#include "combatant/Enemy.h"
#include <iostream>
#include <cstdlib>

Enemy::Enemy() : Combatant(), skillCount(0), patternIndex(0) {}

Enemy::Enemy(const std::string& name, int maxHP, int atk,
             int def, int mgc, Track track)
    : Combatant(name, maxHP, atk, def, mgc, track), skillCount(0), patternIndex(0) {
    Card defaultAtk(0, "공격", "기본 공격", Track::None, TargetScope::Single);
    defaultAtk.addEffect(Effect(EffectType::PhysAttack, atk));
    skills[skillCount++] = EnemySkill(defaultAtk);
}

Enemy::Enemy(const EnemyDef& def)
    : Combatant(def), skillCount(def.skillCount), patternIndex(0) {
    for (int i = 0; i < def.skillCount; ++i) skills[i] = def.skills[i];
    if (skillCount == 0) {
        Card defaultAtk(0, "공격", "기본 공격", Track::None, TargetScope::Single);
        defaultAtk.addEffect(Effect(EffectType::PhysAttack, def.baseAttack));
        skills[skillCount++] = EnemySkill(defaultAtk);
    }
}

int               Enemy::getSkillCount()  const { return skillCount; }
const EnemySkill& Enemy::getSkill(int i)  const {
    static EnemySkill empty;
    return (i >= 0 && i < skillCount) ? skills[i] : empty;
}

EnemySkill Enemy::rollNextSkill() {
    if (skillCount == 0) return EnemySkill();
    EnemySkill next = skills[patternIndex % skillCount];
    patternIndex = (patternIndex + 1) % skillCount;
    return next;
}

void Enemy::prepareIntent() {
    intentQueue.clear();
    intentQueue.enqueue(rollNextSkill());
}

bool Enemy::executeAndQueue(EnemySkill& out) {
    if (!intentQueue.dequeue(out)) return false;
    intentQueue.enqueue(rollNextSkill());
    return true;
}

bool Enemy::peekIntent(EnemySkill& out) const {
    return intentQueue.peek(out);
}

void Enemy::printStatus() const {
    Combatant::printStatus();
    EnemySkill next;
    if (peekIntent(next))
        std::cout << "    Next: [" << next.card.getName() << "]\n";
    else
        std::cout << "    Next: ???\n";
}
