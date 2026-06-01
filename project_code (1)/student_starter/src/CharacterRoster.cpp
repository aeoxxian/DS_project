#include "ds/CharacterRoster.h"
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

bool CharacterRoster::findById(int id, CharacterDef& out) const {
    for (int i = 0; i < count; ++i)
        if (entries[i].id == id) { out = entries[i]; return true; }
    return false;
}

int CharacterRoster::size() const { return count; }

void CharacterRoster::printAll() const {
    for (int i = 0; i < count; ++i) {
        std::cout << "  [" << i << "] " << entries[i].name
                  << "  HP:" << entries[i].baseHp
                  << "  [" << trackToString(entries[i].track) << "]"
                  << "  " << entries[i].description << "\n";
    }
}

// ── 캐릭터 정의 ───────────────────────────────────────────────────────────────
// 캐릭터 추가: CharacterDef(id, 이름, 설명, baseHp, Track) 한 줄
// 캐릭터 1명 = 트랙 1개 고정

void registerAllCharacters() {
    CharacterRoster& r = CharacterRoster::instance();

    r.registerCharacter(CharacterDef(1, "Reactor",   "핵 에너지를 다루는 전문가.",       100, Track::Nuclear));
    r.registerCharacter(CharacterDef(2, "Weaver",    "신소재 융합 기술자.",               90,  Track::NewMaterial));
    r.registerCharacter(CharacterDef(3, "Conductor", "그리드 시스템 총괄 운영자.",         95,  Track::Grid));
    r.registerCharacter(CharacterDef(4, "Drifter",   "수소 현장 전문가.",                 85,  Track::Hydrogen));
    r.registerCharacter(CharacterDef(5, "Architect", "환경기후기술 설계 분석가.",         110,  Track::EcoTech));
    r.registerCharacter(CharacterDef(6, "Ghost",     "AI 기반 은밀 침투 요원.",            80,  Track::AI));
}
