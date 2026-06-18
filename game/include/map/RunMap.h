#ifndef RUN_MAP_H
#define RUN_MAP_H

#include "core/Constants.h"
#include "map/DungeonGraph.h"
#include "ds/Stack.h"

enum class RoomType { Start, Battle, Event, Rest, Stairs, Boss };

struct Position {
    int floor, row, col;
    Position() : floor(0), row(0), col(0) {}
    Position(int f, int r, int c) : floor(f), row(r), col(c) {}
};

class RunMap {
private:
    static const RoomType LAYOUT[MAP_FLOORS][MAP_ROWS][MAP_COLS];

    DungeonGraph    graph;
    bool            cleared[MAP_FLOORS][MAP_ROWS][MAP_COLS];
    Position        cur;
    Stack<Position> history;

    int  roomId(int f, int r, int c) const;
    void buildGraph();

public:
    RunMap();

    bool     move(char dir);       // 'w'/'a'/'s'/'d' — 이동 성공 시 true
    bool     undoMove();
    bool     advanceFloor();       // 계단방에서 다음 층으로

    RoomType currentType()  const;
    bool     isAtBoss()     const;
    bool     isAtStairs()   const;
    bool     isCleared()    const;
    void     clearCurrent();
    void     printMap()     const;
    void     printGraph()   const; // DungeonGraph DFS 출력 (루브릭용)
};

#endif
