#ifndef TRACK_H
#define TRACK_H

#include <string>

// The 6 available tracks a character can specialize in.
// Each character selects exactly 3 tracks at the start of a run.
enum class Track {
    Nuclear = 0,
    NewMaterial = 1,
    Hydrogen = 2,
    EcoTech = 3,
    AI = 4,
    Grid = 5,
    None = 6   // used by normal (non-track) cards
};

std::string trackToString(Track track);
Track parseTrack(const std::string& text);

#endif
