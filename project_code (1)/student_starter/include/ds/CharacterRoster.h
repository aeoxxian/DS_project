#ifndef CHARACTER_ROSTER_H
#define CHARACTER_ROSTER_H

#include "CharacterDef.h"
#include "Constants.h"

class CharacterRoster {
private:
    CharacterDef entries[MAX_ROSTER_SIZE];
    int count;
    CharacterRoster();

public:
    static CharacterRoster& instance();

    bool registerCharacter(const CharacterDef& def);
    bool getAt(int index, CharacterDef& out) const;
    bool findById(int id, CharacterDef& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllCharacters();

#endif
