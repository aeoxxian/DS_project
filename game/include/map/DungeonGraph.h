#ifndef DUNGEON_GRAPH_H
#define DUNGEON_GRAPH_H

#include "map/Room.h"
#include "core/Direction.h"
#include "core/Constants.h"
#include "ds/Stack.h"

class DungeonGraph {
private:
    Room rooms[MAX_ROOMS];
    int  roomCount;
    int  adjacency[MAX_ROOMS][4]; // [roomId][directionToIndex()] = neighborId (-1 = 없음)

public:
    DungeonGraph();

    int   addRoom(const std::string& name, const std::string& description);
    bool  connectRooms(int fromId, Direction dir, int toId, bool bidirectional = true);
    int   getNeighbor(int roomId, Direction dir) const;
    Room* getRoom(int roomId);
    const Room* getRoom(int roomId) const;

    // Stack 기반 DFS — startId부터 탐색, visitedOut[i]=true이면 방문
    void dfs(int startId, bool visitedOut[MAX_ROOMS]) const;
    void printMap() const;
};

#endif
