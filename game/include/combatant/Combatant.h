#ifndef COMBATANT_H
#define COMBATANT_H

#include "core/Track.h"
#include "combatant/CombatantDef.h"
#include "effect/StatusEffect.h"
#include "effect/Effect.h"
#include <string>

class Combatant {
protected:
    std::string name;
    int HP, maxHP;
    int attackPower, defend, magicPower;
    Track track;
    StatusTracker status;

public:
    Combatant();
    Combatant(const std::string& name, int maxHP, int atk,
              int def, int mgc, Track track);
    explicit Combatant(const CombatantDef& def);

    std::string getName()        const;
    int  getHP()                 const;
    int  getMaxHP()              const;
    int  getAttackPower()        const;
    int  getDefend()             const;
    int  getMagicPower()         const;
    Track getTrack()             const;
    bool  hasTrack(Track t)      const;
    bool  isAlive()              const;

    void takeDamage(int amount);
    int  receiveDamage(int raw, EffectType type);
    void heal(int amount);

    StatusTracker&       getStatus();
    const StatusTracker& getStatus() const;
    void tickStatus();
    void clearStatus();

    virtual void printStatus() const;
    virtual ~Combatant() {}
};

#endif
