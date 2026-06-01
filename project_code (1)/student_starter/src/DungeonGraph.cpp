#include "ds/DungeonGraph.h"
#include <iostream>

DungeonGraph::DungeonGraph() : roomCount(0) {
    for (int i = 0; i < MAX_ROOMS; ++i) {
        rooms[i] = nullptr;
        for (int d = 0; d < DIRECTION_COUNT; ++d) {
            adjacency[i][d] = -1;
        }
    }
}

DungeonGraph::~DungeonGraph() {
    for (int i = 0; i < roomCount; ++i) {
        delete rooms[i];
        rooms[i] = nullptr;
    }
}

bool DungeonGraph::isValidRoomId(int roomId) const {
    return roomId >= 0 && roomId < roomCount && rooms[roomId] != nullptr;
}

int DungeonGraph::addRoom(const std::string& name, const std::string& description) {
    if (roomCount >= MAX_ROOMS) {
        return -1;
    }

    int newId = roomCount;
    rooms[newId] = new Room(newId, name, description);
    ++roomCount;
    return newId;
}

bool DungeonGraph::connectRooms(int fromRoomId, Direction direction, int toRoomId, bool bidirectional) {
    // TODO(Student): Implement graph edge insertion.
    // Suggested representation: adjacency[fromRoomId][directionIndex] = toRoomId.
    // If bidirectional is true, also connect toRoomId back to fromRoomId using the opposite direction.
    (void)fromRoomId;
    (void)direction;
    (void)toRoomId;
    (void)bidirectional;
    return false;
}

int DungeonGraph::getNeighbor(int fromRoomId, Direction direction) const {
    // TODO(Student): Implement graph neighbor lookup.
    // Return -1 if the room ID or direction is invalid, or if no edge exists.
    (void)fromRoomId;
    (void)direction;
    return -1;
}

Room* DungeonGraph::getRoom(int roomId) {
    if (!isValidRoomId(roomId)) {
        return nullptr;
    }
    return rooms[roomId];
}

const Room* DungeonGraph::getRoom(int roomId) const {
    if (!isValidRoomId(roomId)) {
        return nullptr;
    }
    return rooms[roomId];
}

int DungeonGraph::size() const {
    return roomCount;
}

void DungeonGraph::printMap() const {
    // TODO(Student): Implement a readable adjacency display.
    // Example:
    // Entrance Hall: east -> Library, south -> Armory
    std::cout << "[TODO] DungeonGraph::printMap is not implemented yet.\n";
}
