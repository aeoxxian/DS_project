#include "map/RunMap.h"
#include "core/UI.h"
#include <iostream>
#include <string>

// ── 고정 레이아웃 (PDF 명세) ──────────────────────────────────────────────────
// [floor][row][col]   row 0 = 상단, col 0 = 좌측

const RoomType RunMap::LAYOUT[MAP_FLOORS][MAP_ROWS][MAP_COLS] = {
    // 1층
    {{ RoomType::Start,  RoomType::Event,  RoomType::Battle },
     { RoomType::Rest,   RoomType::Battle, RoomType::Battle },
     { RoomType::Event,  RoomType::Rest,   RoomType::Stairs }},
    // 2층
    {{ RoomType::Start,  RoomType::Battle, RoomType::Event  },
     { RoomType::Rest,   RoomType::Battle, RoomType::Battle },
     { RoomType::Event,  RoomType::Battle, RoomType::Stairs }},
    // 3층
    {{ RoomType::Start,  RoomType::Event,  RoomType::Rest   },
     { RoomType::Battle, RoomType::Battle, RoomType::Battle },
     { RoomType::Event,  RoomType::Rest,   RoomType::Boss   }},
};

// ── 내부 유틸 ─────────────────────────────────────────────────────────────────

int RunMap::roomId(int f, int r, int c) const { return f * 9 + r * 3 + c; }

static const char* roomLabel(RoomType t) {
    switch (t) {
        case RoomType::Start:   return "시작";
        case RoomType::Battle:  return "전투";
        case RoomType::Event:   return "이벤트";
        case RoomType::Rest:    return "휴식";
        case RoomType::Stairs:  return "계단";
        case RoomType::Boss:    return "보스";
    }
    return "??";
}

static const char* roomSymbol(RoomType t) {
    switch (t) {
        case RoomType::Start:  return " S ";
        case RoomType::Battle: return " B ";
        case RoomType::Event:  return " E ";
        case RoomType::Rest:   return " R ";
        case RoomType::Stairs: return " ^ ";  // ▲ 대체
        case RoomType::Boss:   return " X ";
    }
    return " ? ";
}

// ── 그래프 구축 ───────────────────────────────────────────────────────────────

void RunMap::buildGraph() {
    // 방 27개 등록: floor*9 + row*3 + col 순서
    for (int f = 0; f < MAP_FLOORS; ++f)
        for (int r = 0; r < MAP_ROWS; ++r)
            for (int c = 0; c < MAP_COLS; ++c)
                graph.addRoom(
                    std::string(roomLabel(LAYOUT[f][r][c])) +
                    " F" + std::to_string(f + 1) +
                    "(" + std::to_string(r) + "," + std::to_string(c) + ")",
                    "");

    // 층 내 상하좌우 연결 (단방향 × 2 = 양방향)
    for (int f = 0; f < MAP_FLOORS; ++f)
        for (int r = 0; r < MAP_ROWS; ++r)
            for (int c = 0; c < MAP_COLS; ++c) {
                int id = roomId(f, r, c);
                if (r > 0)              graph.connectRooms(id, Direction::North, roomId(f, r-1, c), false);
                if (r < MAP_ROWS - 1)   graph.connectRooms(id, Direction::South, roomId(f, r+1, c), false);
                if (c > 0)              graph.connectRooms(id, Direction::West,  roomId(f, r, c-1), false);
                if (c < MAP_COLS - 1)   graph.connectRooms(id, Direction::East,  roomId(f, r, c+1), false);
            }
}

// ── 생성자 ────────────────────────────────────────────────────────────────────

RunMap::RunMap() : cur(0, 0, 0) {
    for (int f = 0; f < MAP_FLOORS; ++f)
        for (int r = 0; r < MAP_ROWS; ++r)
            for (int c = 0; c < MAP_COLS; ++c)
                cleared[f][r][c] = false;
    cleared[0][0][0] = true;  // 1층 시작방은 방문 처리
    buildGraph();
}

// ── 이동 ─────────────────────────────────────────────────────────────────────

bool RunMap::move(char dir) {
    Direction d;
    if      (dir == 'w') d = Direction::North;
    else if (dir == 's') d = Direction::South;
    else if (dir == 'a') d = Direction::West;
    else if (dir == 'd') d = Direction::East;
    else return false;

    int nextId = graph.getNeighbor(roomId(cur.floor, cur.row, cur.col), d);
    if (nextId < 0) {
        std::cout << "  그 방향으로는 이동할 수 없습니다.\n";
        return false;
    }

    history.push(cur);
    cur = Position(nextId / 9, (nextId % 9) / 3, nextId % 3);
    return true;
}

bool RunMap::undoMove() {
    Position prev;
    if (!history.pop(prev)) return false;
    cur = prev;
    return true;
}

bool RunMap::advanceFloor() {
    if (!isAtStairs() || cur.floor >= MAP_FLOORS - 1) return false;
    history.push(cur);
    cur = Position(cur.floor + 1, 0, 0);
    cleared[cur.floor][0][0] = true;  // 다음 층 시작방 방문 처리
    return true;
}

// ── 상태 조회 ─────────────────────────────────────────────────────────────────

RoomType RunMap::currentType()  const { return LAYOUT[cur.floor][cur.row][cur.col]; }
bool     RunMap::isAtBoss()     const { return currentType() == RoomType::Boss; }
bool     RunMap::isAtStairs()   const { return currentType() == RoomType::Stairs; }
bool     RunMap::isCleared()    const { return cleared[cur.floor][cur.row][cur.col]; }
void     RunMap::clearCurrent()       { cleared[cur.floor][cur.row][cur.col] = true; }

// ── 출력 ─────────────────────────────────────────────────────────────────────

void RunMap::printMap() const {
    static const int W = 50;
    int f = cur.floor;

    auto cell = [&](int r, int c) -> std::string {
        bool here = (cur.row == r && cur.col == c);
        bool done = cleared[f][r][c]
                    && LAYOUT[f][r][c] != RoomType::Start
                    && LAYOUT[f][r][c] != RoomType::Stairs;
        if (here) return "[ @ ]";
        if (done) return "[ * ]";
        return std::string("[") + roomSymbol(LAYOUT[f][r][c]) + "]";
    };

    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter(std::to_string(f + 1) + "층  |  지식의 상아탑", W);
    UI::boxDiv(W);
    UI::boxEmpty(W);
    for (int r = 0; r < 3; ++r) {
        UI::boxLeft("  " + cell(r,0) + "---" + cell(r,1) + "---" + cell(r,2), W);
        if (r < 2) UI::boxLeft("    |       |       |", W);
    }
    UI::boxEmpty(W);
    UI::boxDiv(W);
    UI::boxLeft("  @현재  *완료  B전투  E이벤트", W);
    UI::boxLeft("  R휴식  ^계단  X보스  S시작", W);
    UI::boxLeft("  w/a/s/d 이동    u 취소    h 도움말", W);
    UI::boxBot(W);
    std::cout << "\n";
}

void RunMap::printGraph() const {
    graph.printMap();
}
