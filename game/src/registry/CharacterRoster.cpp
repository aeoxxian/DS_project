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

    r.registerCharacter(CharacterDef(1, "이민준", "Nuclear 트랙",     100, 0, 0, Track::Nuclear));
    r.registerCharacter(CharacterDef(2, "김서연", "AI 트랙",          100, 0, 0, Track::AI));
    r.registerCharacter(CharacterDef(3, "박준서", "Grid 트랙",        100, 0, 0, Track::Grid));
    r.registerCharacter(CharacterDef(4, "최유진", "NewMaterial 트랙", 100, 0, 0, Track::NewMaterial));
    r.registerCharacter(CharacterDef(5, "정하은", "Hydrogen 트랙",    100, 0, 0, Track::Hydrogen));
    r.registerCharacter(CharacterDef(6, "한승준", "EcoTech 트랙",     100, 0, 0, Track::EcoTech));
}
