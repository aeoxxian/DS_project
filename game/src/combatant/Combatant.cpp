#include "combatant/Combatant.h"
#include <iostream>

Combatant::Combatant()
    : name(""), HP(0), maxHP(0), attackPower(0),
      defend(0), magicPower(0), track(Track::None) {}

Combatant::Combatant(const std::string& name, int maxHP, int attackPower,
                     int defend, int magicPower, Track track)
    : name(name), HP(maxHP), maxHP(maxHP), attackPower(attackPower),
      defend(defend), magicPower(magicPower), track(track) {}

Combatant::Combatant(const CombatantDef& def)
    : name(def.name), HP(def.baseHP), maxHP(def.baseHP),
      attackPower(def.baseAttack), defend(def.baseDefend),
      magicPower(def.baseMagicPower),
      track(def.track) {}

std::string Combatant::getName()        const { return name; }
int  Combatant::getHP()                 const { return HP; }
int  Combatant::getMaxHP()              const { return maxHP; }
int  Combatant::getAttackPower()        const { return attackPower + status.getModifier("atk_up")  - status.getModifier("atk_down"); }
int  Combatant::getDefend()             const { return defend      + status.getModifier("def_up")  - status.getModifier("def_down"); }
int  Combatant::getMagicPower()         const { return magicPower  + status.getModifier("mag_up")  - status.getModifier("mag_down"); }
Track Combatant::getTrack()             const { return track; }
bool  Combatant::hasTrack(Track t)      const { return track == t; }
bool  Combatant::isAlive()              const { return HP > 0; }

void Combatant::takeDamage(int amount)  { HP = (HP - amount < 0) ? 0 : HP - amount; }

int Combatant::receiveDamage(int raw, EffectType type) {
    (void)type;
    if (status.has("vulnerable")) raw = raw * 150 / 100;
    int actual = raw - status.getModifier("shield");
    if (actual < 0) actual = 0;
    takeDamage(actual);
    return actual;
}

void Combatant::heal(int amount) { HP = (HP + amount > maxHP) ? maxHP : HP + amount; }

StatusTracker&       Combatant::getStatus()       { return status; }
const StatusTracker& Combatant::getStatus() const  { return status; }
void Combatant::tickStatus()          { status.tick(); }
void Combatant::clearStatus()         { status.clear(); }

void Combatant::printStatus() const {
    std::cout << name
              << "  HP:" << HP << "/" << maxHP
              << "  ATK:" << getAttackPower()
              << "  DEF:" << getDefend()
              << "  MGC:" << getMagicPower()
              << "  [" << trackToString(track) << "]";
    status.print();
    std::cout << "\n";
}
