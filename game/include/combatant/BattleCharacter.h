#ifndef BATTLE_CHARACTER_H
#define BATTLE_CHARACTER_H

#include "combatant/Combatant.h"
#include "card/Card.h"
#include "registry/CharacterRoster.h"

class BattleCharacter : public Combatant {
private:
    bool hasAssignedCard;
    Card assignedCard;

public:
    BattleCharacter();
    BattleCharacter(const std::string& name, int maxHP, int atk, int def, Track track);
    BattleCharacter(const CharacterDef& def);

    void        assignCard(const Card& card);
    bool        hasCard() const;
    const Card& getAssignedCard() const;
    void        clearAssignedCard();

    void printStatus() const override;
};

#endif
