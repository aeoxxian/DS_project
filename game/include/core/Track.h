#ifndef TRACK_H
#define TRACK_H

#include <string>

enum class Track {
    None,
    Nuclear,
    NewMaterial,
    Hydrogen,
    EcoTech,
    AI,
    Grid
};

inline std::string trackToString(Track t) {
    switch (t) {
        case Track::Nuclear:     return "핵공학";
        case Track::NewMaterial: return "신소재";
        case Track::Hydrogen:    return "수소";
        case Track::EcoTech:     return "환경기후기술";
        case Track::AI:          return "인공지능";
        case Track::Grid:        return "스마트그리드";
        default:                 return "공용";
    }
}

#endif
