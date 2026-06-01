#ifndef RUN_MAP_H
#define RUN_MAP_H

#include "MapNode.h"

static const int MAX_MAP_NODES = 48;
static const int MAP_DEPTH     = 9;   // 트리 깊이 — 경로당 방 10개 (depth 0~8 + 보스)

class RunMap {
private:
    MapNode nodes[MAX_MAP_NODES];
    int     nodeCount;
    int     currentNodeId;

    int  addNode(int depth, bool isBoss = false);
    void buildMap();
    RoomType rollRoomType() const; // 진입 시 랜덤 결정

public:
    RunMap();

    // 현재 노드의 자식 중 하나를 선택해 이동.
    // 이동 후 해당 방의 RoomType(랜덤)을 반환.
    RoomType advance();

    // 현재 방 타입 (진입 시 결정됨)
    RoomType currentType() const;
    int      currentId()   const;
    bool     isAtBoss()    const;

    void printMap() const;

private:
    RoomType roomTypes[MAX_MAP_NODES]; // 각 노드 진입 시 결정된 타입 캐싱
};

#endif
