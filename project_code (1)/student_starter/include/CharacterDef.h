#ifndef CHARACTER_DEF_H
#define CHARACTER_DEF_H

#include "Track.h"
#include <string>

// 캐릭터 고유 정의 — 트랙 1개 고정
struct CharacterDef {
    int         id;
    std::string name;
    std::string description;
    int         baseHp;
    Track       track;     // 캐릭터 고유 트랙 (1개)

    CharacterDef() : id(0), baseHp(0), track(Track::None) {}
    CharacterDef(int id, const std::string& name,
                 const std::string& desc, int baseHp, Track track)
        : id(id), name(name), description(desc), baseHp(baseHp), track(track) {}
};

#endif
