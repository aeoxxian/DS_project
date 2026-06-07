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
        case Track::Nuclear:     return "Nuclear";
        case Track::NewMaterial: return "NewMaterial";
        case Track::Hydrogen:    return "Hydrogen";
        case Track::EcoTech:     return "EcoTech";
        case Track::AI:          return "AI";
        case Track::Grid:        return "Grid";
        default:                 return "None";
    }
}

#endif
