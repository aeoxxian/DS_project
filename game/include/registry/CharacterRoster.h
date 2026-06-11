#ifndef CHARACTER_ROSTER_H
#define CHARACTER_ROSTER_H

#include "combatant/CombatantDef.h"
#include "core/Constants.h"
#include <string>

struct CharacterDef : public CombatantDef {
    int         id;
    std::string description;

    CharacterDef() : CombatantDef(), id(0) {}
    CharacterDef(int id, const std::string& name, const std::string& desc,
                 int hp, int atk, int def, Track track)
        : CombatantDef(name, hp, atk, def, track),
          id(id), description(desc) {}
};

class CharacterRoster {
private:
    CharacterDef entries[MAX_ROSTER_SIZE];
    int          count;
    CharacterRoster();

public:
    static CharacterRoster& instance();
    bool registerCharacter(const CharacterDef& def);
    bool getAt(int index, CharacterDef& out) const;
    bool findByName(const std::string& name, CharacterDef& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllCharacters();

#endif
