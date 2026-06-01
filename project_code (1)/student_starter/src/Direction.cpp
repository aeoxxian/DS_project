#include "Direction.h"

Direction parseDirection(const std::string& text) {
    if (text == "north" || text == "n") {
        return Direction::North;
    }
    if (text == "south" || text == "s") {
        return Direction::South;
    }
    if (text == "east" || text == "e") {
        return Direction::East;
    }
    if (text == "west" || text == "w") {
        return Direction::West;
    }
    return Direction::Invalid;
}

std::string directionToString(Direction direction) {
    switch (direction) {
        case Direction::North: return "north";
        case Direction::South: return "south";
        case Direction::East: return "east";
        case Direction::West: return "west";
        default: return "invalid";
    }
}

Direction oppositeDirection(Direction direction) {
    switch (direction) {
        case Direction::North: return Direction::South;
        case Direction::South: return Direction::North;
        case Direction::East: return Direction::West;
        case Direction::West: return Direction::East;
        default: return Direction::Invalid;
    }
}

int directionToIndex(Direction direction) {
    switch (direction) {
        case Direction::North: return 0;
        case Direction::South: return 1;
        case Direction::East: return 2;
        case Direction::West: return 3;
        default: return -1;
    }
}
