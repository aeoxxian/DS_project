#include "registry/CharacterRoster.h"
#include <iostream>

CharacterRoster::CharacterRoster() : count(0) {}

CharacterRoster& CharacterRoster::instance() {
    static CharacterRoster roster;
    return roster;
}

bool CharacterRoster::registerCharacter(const CharacterDef& def) {
    if (count >= MAX_ROSTER_SIZE) return false;
    entries[count++] = def;
    return true;
}

bool CharacterRoster::getAt(int index, CharacterDef& out) const {
    if (index < 0 || index >= count) return false;
    out = entries[index]; return true;
}

int CharacterRoster::size() const { return count; }

void CharacterRoster::printAll() const {
    std::cout << "=== 캐릭터 로스터 (" << count << ") ===\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  [" << i << "] " << entries[i].name
                  << " [" << trackToString(entries[i].track) << "]"
                  << "  HP:" << entries[i].baseHP
                  << "  ATK:" << entries[i].baseAttack << "\n";
}

// ── 캐릭터 정의 ───────────────────────────────────────────────────────────────
// CharacterDef(id, 이름, 설명, HP, ATK, DEF, MGC, Track)

void registerAllCharacters() {
    CharacterRoster& r = CharacterRoster::instance();

    r.registerCharacter(CharacterDef(1, "캐릭터1", "팀원 A", 100, 10, 8, 6, Track::Nuclear));
    r.registerCharacter(CharacterDef(2, "캐릭터2", "팀원 B", 90,  8,  6, 12, Track::AI));
    r.registerCharacter(CharacterDef(3, "캐릭터3", "팀원 C", 95,  9,  7, 9,  Track::Grid));
}
