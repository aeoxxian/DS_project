#include "Enemy.h"
#include <iostream>
#include <cstdlib>

Enemy::Enemy()
    : name(""), hp(0), maxHp(0), attackPower(0),
      track(Track::None), currentIntent(EnemyIntent::Unknown) {}

Enemy::Enemy(const std::string& name, int maxHp, int attackPower, Track track)
    : name(name), hp(maxHp), maxHp(maxHp), attackPower(attackPower),
      track(track), currentIntent(EnemyIntent::Unknown) {}

std::string Enemy::getName() const { return name; }
int Enemy::getHp() const { return hp; }
int Enemy::getMaxHp() const { return maxHp; }
int Enemy::getAttackPower() const {
    // 정식 키 기반: 공격력 버프/디버프를 가/감산. weaken%는 Battle의 데미지 라우팅에서 적용.
    int mod = status.getModifier(ST_ATK_UP) - status.getModifier(ST_ATK_DOWN);
    return (attackPower + mod < 0) ? 0 : attackPower + mod;
}
Track Enemy::getTrack() const { return track; }
EnemyIntent Enemy::getIntent() const { return currentIntent; }
bool Enemy::isAlive() const { return hp > 0; }

void Enemy::takeDamage(int amount) { hp = (hp - amount < 0) ? 0 : hp - amount; }

void Enemy::decideIntent() {
    int roll = rand() % 10;
    if (roll < 7)      currentIntent = EnemyIntent::Attack;
    else if (roll < 9) currentIntent = EnemyIntent::Defend;
    else               currentIntent = EnemyIntent::Buff;
}

int Enemy::executeIntent() {
    switch (currentIntent) {
        case EnemyIntent::Attack:
            std::cout << "  " << name << " attacks for " << getAttackPower() << "!\n";
            return getAttackPower();
        case EnemyIntent::Defend:
            std::cout << "  " << name << " braces.\n";
            return 0;
        case EnemyIntent::Buff:
            std::cout << "  " << name << " powers up!\n";
            return 0;
        default: return 0;
    }
}

StatusTracker& Enemy::getStatus() { return status; }
void Enemy::tickStatus() { status.tick(); }

void Enemy::printStatus() const {
    std::cout << name << "  HP:" << hp << "/" << maxHp
              << "  [" << trackToString(track) << "]"
              << "  " << intentToString();
    status.print();
    std::cout << "\n";
}

std::string Enemy::intentToString() const {
    switch (currentIntent) {
        case EnemyIntent::Attack:  return "ATK";
        case EnemyIntent::Defend:  return "DEF";
        case EnemyIntent::Buff:    return "BUFF";
        default:                   return "???";
    }
}
