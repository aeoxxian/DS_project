#include "Track.h"

std::string trackToString(Track track) {
    switch (track) {
        case Track::Nuclear:     return "Nuclear";
        case Track::NewMaterial: return "NewMaterial";
        case Track::Hydrogen:    return "Hydrogen";
        case Track::EcoTech:     return "EcoTech";
        case Track::AI:          return "AI";
        case Track::Grid:        return "Grid";
        default:                 return "None";
    }
}

Track parseTrack(const std::string& text) {
    if (text == "nuclear")     return Track::Nuclear;
    if (text == "newmaterial") return Track::NewMaterial;
    if (text == "hydrogen")    return Track::Hydrogen;
    if (text == "ecotech")     return Track::EcoTech;
    if (text == "ai")          return Track::AI;
    if (text == "grid")        return Track::Grid;
    return Track::None;
}
