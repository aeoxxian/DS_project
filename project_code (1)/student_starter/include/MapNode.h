#ifndef MAP_NODE_H
#define MAP_NODE_H

#include "Constants.h"

enum class RoomType { Battle, Event, Rest, Boss };

static const int MAX_CHILDREN = 3;

struct MapNode {
    int  id;
    int  depth;
    bool visited;
    bool isBoss;

    int children[MAX_CHILDREN];
    int childCount;

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

#endif
