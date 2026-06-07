#ifndef REGISTRIES_H
#define REGISTRIES_H

#include "registry/CardRegistry.h"
#include "registry/CharacterRoster.h"
#include "registry/MonsterRegistry.h"
#include "registry/EventRegistry.h"

inline void registerAll() {
    registerAllCards();
    registerAllCharacters();
    registerAllMonsters();
    registerAllEvents();
}

#endif
