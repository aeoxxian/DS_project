#include "BattleCharacter.h"
#include <iostream>

BattleCharacter::BattleCharacter()
    : name(""), hp(0), maxHp(0), track(Track::None), hasAssignedCard(false) {}

BattleCharacter::BattleCharacter(const std::string& name, int maxHp, Track track)
    : name(name), hp(maxHp), maxHp(maxHp), track(track), hasAssignedCard(false) {}

BattleCharacter::BattleCharacter(const CharacterDef& def)
    : name(def.name), hp(def.baseHp), maxHp(def.baseHp),
      track(def.track), hasAssignedCard(false) {}

std::string BattleCharacter::getName() const { return name; }
int  BattleCharacter::getHp()    const { return hp; }
int  BattleCharacter::getMaxHp() const { return maxHp; }
bool BattleCharacter::isAlive()  const { return hp > 0; }

void BattleCharacter::takeDamage(int amount) { hp = (hp - amount < 0) ? 0 : hp - amount; }
void BattleCharacter::heal(int amount)       { hp = (hp + amount > maxHp) ? maxHp : hp + amount; }

Track BattleCharacter::getTrack()        const { return track; }
bool  BattleCharacter::hasTrack(Track t) const { return track == t; }

void BattleCharacter::assignCard(const Card& card) { assignedCard = card; hasAssignedCard = true; }
bool BattleCharacter::hasCard()                    const { return hasAssignedCard; }
const Card& BattleCharacter::getAssignedCard()     const { return assignedCard; }
void BattleCharacter::clearAssignedCard()                { hasAssignedCard = false; }

StatusTracker& BattleCharacter::getStatus() { return status; }
void BattleCharacter::tickStatus()          { status.tick(); }

void BattleCharacter::printStatus() const {
    std::cout << name << "  HP:" << hp << "/" << maxHp
              << "  [" << trackToString(track) << "]";
    status.print();
    std::cout << "\n";
}
