#include "map/DungeonGraph.h"
#include "core/Direction.h"
#include <iostream>

DungeonGraph::DungeonGraph() : roomCount(0) {
    for (int i = 0; i < MAX_ROOMS; ++i)
        for (int j = 0; j < 4; ++j)
            adjacency[i][j] = -1;
}

int DungeonGraph::addRoom(const std::string& name, const std::string& description) {
    if (roomCount >= MAX_ROOMS) return -1;
    int id = roomCount++;
    rooms[id] = Room(id, name, description);
    return id;
}

bool DungeonGraph::connectRooms(int fromId, Direction dir, int toId, bool bidirectional) {
    if (fromId < 0 || fromId >= roomCount) return false;
    if (toId   < 0 || toId   >= roomCount) return false;
    adjacency[fromId][directionToIndex(dir)] = toId;
    if (bidirectional)
        adjacency[toId][directionToIndex(oppositeDirection(dir))] = fromId;
    return true;
}

int DungeonGraph::getNeighbor(int roomId, Direction dir) const {
    if (roomId < 0 || roomId >= roomCount) return -1;
    return adjacency[roomId][directionToIndex(dir)];
}

Room* DungeonGraph::getRoom(int roomId) {
    if (roomId < 0 || roomId >= roomCount) return nullptr;
    return &rooms[roomId];
}

const Room* DungeonGraph::getRoom(int roomId) const {
    if (roomId < 0 || roomId >= roomCount) return nullptr;
    return &rooms[roomId];
}

void DungeonGraph::dfs(int startId, bool visitedOut[MAX_ROOMS]) const {
    for (int i = 0; i < MAX_ROOMS; ++i) visitedOut[i] = false;
    if (startId < 0 || startId >= roomCount) return;

    Stack<int> stack;
    stack.push(startId);

    while (!stack.isEmpty()) {
        int id;
        stack.pop(id);
        if (visitedOut[id]) continue;
        visitedOut[id] = true;

        // 역순 push → North부터 탐색
        for (int d = 3; d >= 0; --d) {
            int neighbor = adjacency[id][d];
            if (neighbor != -1 && !visitedOut[neighbor])
                stack.push(neighbor);
        }
    }
}

void DungeonGraph::printMap() const {
    if (roomCount == 0) { std::cout << "(맵 없음)\n"; return; }

    bool visited[MAX_ROOMS];
    dfs(0, visited);

    std::cout << "=== 던전 맵 ===\n";
    for (int id = 0; id < roomCount; ++id) {
        if (!visited[id]) continue;
        const Room* r = getRoom(id);
        std::cout << "  [" << id << "] " << r->getName();
        if (r->hasBeenVisited()) std::cout << " *";
        std::cout << "\n";
        for (int d = 0; d < 4; ++d) {
            int neighbor = adjacency[id][d];
            if (neighbor == -1) continue;
            Direction dir = static_cast<Direction>(d);
            std::cout << "       " << directionToString(dir)
                      << " -> [" << neighbor << "] "
                      << getRoom(neighbor)->getName() << "\n";
        }
    }
}
