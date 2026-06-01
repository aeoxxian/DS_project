#ifndef BATTLE_CHARACTER_H
#define BATTLE_CHARACTER_H

#include "Constants.h"
#include "CharacterDef.h"
#include "Track.h"
#include "Card.h"
#include "StatusEffect.h"
#include <string>

class BattleCharacter {
private:
    std::string name;
    int hp, maxHp;
    Track track;           // 캐릭터 고유 트랙 1개
    bool hasAssignedCard;
    Card assignedCard;
    StatusTracker status;

public:
    BattleCharacter();
    BattleCharacter(const std::string& name, int maxHp, Track track);
    BattleCharacter(const CharacterDef& def);

    std::string getName() const;
    int  getHp() const;
    int  getMaxHp() const;
    bool isAlive() const;
    void takeDamage(int amount);
    void heal(int amount);

    Track getTrack() const;
    bool  hasTrack(Track t) const; // t == this->track

    void        assignCard(const Card& card);
    bool        hasCard() const;
    const Card& getAssignedCard() const;
    void        clearAssignedCard();

    StatusTracker& getStatus();
    void tickStatus();

    void printStatus() const;
};

#endif
