#ifndef RUN_MAP_H
#define RUN_MAP_H

#include "core/Constants.h"
#include "ds/Stack.h"

enum class RoomType { Battle, Event, Rest, Boss };

struct MapNode {
    int  id, depth;
    bool visited, isBoss;
    int  children[MAX_CHILDREN];
    int  childCount;

    MapNode() : id(-1), depth(0), visited(false), isBoss(false), childCount(0) {
        for (int i = 0; i < MAX_CHILDREN; ++i) children[i] = -1;
    }
    MapNode(int id, int depth, bool isBoss = false)
        : id(id), depth(depth), visited(false), isBoss(isBoss), childCount(0) {
        for (int i = 0; i < MAX_CHILDREN; ++i) children[i] = -1;
    }
    bool addChild(int nodeId) {
        if (childCount >= MAX_CHILDREN) return false;
        children[childCount++] = nodeId;
        return true;
    }
};

class RunMap {
private:
    MapNode      nodes[MAX_MAP_NODES];
    int          nodeCount;
    int          currentNodeId;
    RoomType     roomTypes[MAX_MAP_NODES];
    Stack<int>   moveHistory;

    int      addNode(int depth, bool isBoss = false);
    void     buildMap();
    RoomType rollRoomType() const;

public:
    RunMap();

    RoomType advance();
    bool     undoMove();
    RoomType currentType() const;
    int      currentId()   const;
    bool     isAtBoss()    const;
    void     printMap()    const;
};

#endif
