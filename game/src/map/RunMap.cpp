#include "map/RunMap.h"
#include "core/UI.h"
#include <iostream>
#include <cstdlib>

RunMap::RunMap() : nodeCount(0), currentNodeId(0) {
    for (int i = 0; i < MAX_MAP_NODES; ++i) roomTypes[i] = RoomType::Battle;
    buildMap();
    roomTypes[currentNodeId] = rollRoomType();
}

int RunMap::addNode(int depth, bool isBoss) {
    if (nodeCount >= MAX_MAP_NODES) return -1;
    int id = nodeCount;
    nodes[nodeCount++] = MapNode(id, depth, isBoss);
    return id;
}

RoomType RunMap::rollRoomType() const {
    int r = rand() % 10;
    if (r < 5) return RoomType::Battle;
    if (r < 8) return RoomType::Event;
    return RoomType::Rest;
}

void RunMap::buildMap() {
    int layer[MAP_DEPTH + 1][3];
    int layerSize[MAP_DEPTH + 1];
    for (int i = 0; i <= MAP_DEPTH; ++i) layerSize[i] = 0;

    layer[0][0] = addNode(0);
    layerSize[0] = 1;

    for (int d = 1; d < MAP_DEPTH; ++d) {
        int count = 1 + rand() % 2;
        layerSize[d] = count;
        for (int i = 0; i < count; ++i)
            layer[d][i] = addNode(d);
    }

    layer[MAP_DEPTH][0] = addNode(MAP_DEPTH, true);
    layerSize[MAP_DEPTH] = 1;

    for (int d = 0; d < MAP_DEPTH; ++d)
        for (int p = 0; p < layerSize[d]; ++p)
            for (int c = 0; c < layerSize[d + 1]; ++c)
                nodes[layer[d][p]].addChild(layer[d + 1][c]);

    currentNodeId = layer[0][0];
}

static const char* roomLabel(RoomType t) {
    switch (t) {
        case RoomType::Battle: return "전투  ";
        case RoomType::Event:  return "이벤트";
        case RoomType::Rest:   return "휴식  ";
        case RoomType::Boss:   return "BOSS  ";
    }
    return "??    ";
}

bool RunMap::undoMove() {
    int prevId;
    if (!moveHistory.pop(prevId)) return false;
    nodes[currentNodeId].visited = false;
    currentNodeId = prevId;
    return true;
}

RoomType RunMap::advance() {
    nodes[currentNodeId].visited = true;
    MapNode& cur = nodes[currentNodeId];
    if (cur.childCount == 0) return currentType();

    int prevId = currentNodeId;

    if (cur.childCount == 1) {
        currentNodeId = cur.children[0];
    } else {
        std::cout << "\n  ── 분기 선택 ──────────────────────────────\n";
        for (int i = 0; i < cur.childCount; ++i) {
            int childId = cur.children[i];
            std::cout << "  [" << i << "]  "
                      << (nodes[childId].isBoss ? "▲ BOSS" : "? 미탐색") << "\n";
        }
        std::cout << "\n  선택 > ";
        std::string line;
        std::getline(std::cin, line);
        int choice = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? 0 : std::stoi(line);
        if (choice < 0 || choice >= cur.childCount) choice = 0;
        currentNodeId = cur.children[choice];
    }

    moveHistory.push(prevId);

    if (nodes[currentNodeId].isBoss)
        roomTypes[currentNodeId] = RoomType::Boss;
    else
        roomTypes[currentNodeId] = rollRoomType();

    std::cout << "\n  → 입장: " << roomLabel(roomTypes[currentNodeId]) << "\n";
    UI::pause();
    return roomTypes[currentNodeId];
}

RoomType RunMap::currentType() const { return roomTypes[currentNodeId]; }
int      RunMap::currentId()   const { return currentNodeId; }
bool     RunMap::isAtBoss()    const { return nodes[currentNodeId].isBoss; }

void RunMap::printMap() const {
    int curDepth = nodes[currentNodeId].depth;

    std::cout << "  ═══════════════════════════════════════════════\n";
    std::cout << "  던전 맵  (진행도 " << curDepth << " / " << MAP_DEPTH << ")\n";
    std::cout << "  ───────────────────────────────────────────────\n";

    for (int d = 0; d <= MAP_DEPTH; ++d) {
        std::cout << "  " << d << "  ";
        for (int i = 0; i < nodeCount; ++i) {
            if (nodes[i].depth != d) continue;

            if (nodes[i].id == currentNodeId) {
                std::cout << "[ ★ " << roomLabel(roomTypes[i]) << " ]  ";
            } else if (nodes[i].visited) {
                std::cout << "[ ● " << roomLabel(roomTypes[i]) << " ]  ";
            } else if (nodes[i].isBoss) {
                std::cout << "[ ▲  BOSS     ]  ";
            } else {
                std::cout << "[    ?        ]  ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "  ═══════════════════════════════════════════════\n";
}
