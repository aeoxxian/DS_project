#include "combatant/BattleCharacter.h"
#include <iostream>

BattleCharacter::BattleCharacter() : Combatant(), hasAssignedCard(false) {}

BattleCharacter::BattleCharacter(const std::string& name, int maxHP, int atk,
                                  int def, int mgc, Track track)
    : Combatant(name, maxHP, atk, def, mgc, track), hasAssignedCard(false) {}

BattleCharacter::BattleCharacter(const CharacterDef& def)
    : Combatant(def), hasAssignedCard(false) {}

void BattleCharacter::assignCard(const Card& card) {
    assignedCard    = card;
    hasAssignedCard = true;
}

bool        BattleCharacter::hasCard()           const { return hasAssignedCard; }
const Card& BattleCharacter::getAssignedCard()   const { return assignedCard; }
void        BattleCharacter::clearAssignedCard()       { hasAssignedCard = false; }

void BattleCharacter::printStatus() const {
    Combatant::printStatus();
    if (hasAssignedCard) {
        std::cout << "    Card: "; assignedCard.print(); std::cout << "\n";
    }
}
