#include "RunMap.h"
#include <iostream>
#include <cstdlib>

RunMap::RunMap() : nodeCount(0), currentNodeId(0) {
    for (int i = 0; i < MAX_MAP_NODES; ++i) roomTypes[i] = RoomType::Battle;
    buildMap();
    // 시작 방 타입 결정
    roomTypes[currentNodeId] = rollRoomType();
}

int RunMap::addNode(int depth, bool isBoss) {
    if (nodeCount >= MAX_MAP_NODES) return -1;
    int id = nodeCount;
    nodes[nodeCount++] = MapNode(id, depth, isBoss);
    return id;
}

RoomType RunMap::rollRoomType() const {
    // 보스는 별도 처리, 일반 방: 전투 50% / 이벤트 30% / 휴식 20%
    int r = rand() % 10;
    if (r < 5) return RoomType::Battle;
    if (r < 8) return RoomType::Event;
    return RoomType::Rest;
}

void RunMap::buildMap() {
    // 각 레이어별 노드 id 저장
    int layer[MAP_DEPTH + 1][3];
    int layerSize[MAP_DEPTH + 1];
    for (int i = 0; i <= MAP_DEPTH; ++i) layerSize[i] = 0;

    // depth 0: 시작 노드 1개
    layer[0][0] = addNode(0);
    layerSize[0] = 1;

    // depth 1 ~ MAP_DEPTH-1: 랜덤 1~2개 분기
    for (int d = 1; d < MAP_DEPTH; ++d) {
        int count = 1 + rand() % 2;
        layerSize[d] = count;
        for (int i = 0; i < count; ++i)
            layer[d][i] = addNode(d);
    }

    // depth MAP_DEPTH: 보스 1개
    layer[MAP_DEPTH][0] = addNode(MAP_DEPTH, true);
    layerSize[MAP_DEPTH] = 1;

    // 연결
    for (int d = 0; d < MAP_DEPTH; ++d)
        for (int p = 0; p < layerSize[d]; ++p)
            for (int c = 0; c < layerSize[d + 1]; ++c)
                nodes[layer[d][p]].addChild(layer[d + 1][c]);

    currentNodeId = layer[0][0];
}

RoomType RunMap::advance() {
    nodes[currentNodeId].visited = true;
    MapNode& cur = nodes[currentNodeId];

    if (cur.childCount == 0) return currentType();

    if (cur.childCount == 1) {
        currentNodeId = cur.children[0];
    } else {
        std::cout << "\n-- 다음 방 선택 --\n";
        for (int i = 0; i < cur.childCount; ++i)
            std::cout << "  [" << i << "] 방 " << (cur.depth + 1) << "\n";
        std::cout << "> ";
        std::string line; std::getline(std::cin, line);
        int choice = (line.empty()) ? 0 : std::stoi(line);
        if (choice < 0 || choice >= cur.childCount) choice = 0;
        currentNodeId = cur.children[choice];
    }

    // 진입 시 방 타입 결정 (보스 제외)
    if (nodes[currentNodeId].isBoss)
        roomTypes[currentNodeId] = RoomType::Boss;
    else
        roomTypes[currentNodeId] = rollRoomType();

    return roomTypes[currentNodeId];
}

RoomType RunMap::currentType() const { return roomTypes[currentNodeId]; }
int      RunMap::currentId()   const { return currentNodeId; }
bool     RunMap::isAtBoss()    const { return nodes[currentNodeId].isBoss; }

void RunMap::printMap() const {
    std::cout << "\n=== 맵 (depth " << nodes[currentNodeId].depth << " / " << MAP_DEPTH << ") ===\n";
    for (int d = 0; d <= MAP_DEPTH; ++d) {
        std::cout << "  ";
        for (int i = 0; i < nodeCount; ++i) {
            if (nodes[i].depth != d) continue;
            if (nodes[i].id == currentNodeId) std::cout << "[현재] ";
            else if (nodes[i].visited)        std::cout << "[방문] ";
            else if (nodes[i].isBoss)         std::cout << "[BOSS] ";
            else                              std::cout << "[   ] ";
        }
        std::cout << "\n";
    }
}
