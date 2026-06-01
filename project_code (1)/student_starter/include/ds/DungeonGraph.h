#ifndef DUNGEON_GRAPH_H
#define DUNGEON_GRAPH_H

#include "Direction.h"
#include "Room.h"

class DungeonGraph {
private:
    static const int MAX_ROOMS = 20;
    static const int DIRECTION_COUNT = 4;

    Room* rooms[MAX_ROOMS];
    int adjacency[MAX_ROOMS][DIRECTION_COUNT];
    int roomCount;

    bool isValidRoomId(int roomId) const;

    DungeonGraph(const DungeonGraph& other) = delete;
    DungeonGraph& operator=(const DungeonGraph& other) = delete;

public:
    DungeonGraph();
    ~DungeonGraph();

    int addRoom(const std::string& name, const std::string& description);
    bool connectRooms(int fromRoomId, Direction direction, int toRoomId, bool bidirectional = true);
    int getNeighbor(int fromRoomId, Direction direction) const;
    Room* getRoom(int roomId);
    const Room* getRoom(int roomId) const;
    int size() const;
    void printMap() const;
};

#endif
