#pragma once
#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>

namespace UI {

// ── 기본 ─────────────────────────────────────────────────────────────────────

inline void clear() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

inline void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void pause() {
    std::cout << "\n  [ Enter ] Continue... ";
    std::string s;
    std::getline(std::cin, s);
}

// UTF-8 display width: Korean/CJK 3-byte chars = 2 columns, ASCII = 1
inline int displayWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = (unsigned char)s[i];
        if      (c < 0x80) { i += 1; w += 1; }
        else if (c < 0xE0) { i += 2; w += 1; }
        else if (c < 0xF0) { i += 3; w += 2; }  // Korean/CJK
        else               { i += 4; w += 2; }
    }
    return w;
}

// ── 텍스트 효과 ───────────────────────────────────────────────────────────────

inline void typewrite(const std::string& text, int delayMs = 22) {
    std::cout << "  ";
    for (char c : text) {
        std::cout << c;
        std::cout.flush();
        if (c != ' ' && delayMs > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }
    std::cout << "\n";
}

// ── HP 바 ─────────────────────────────────────────────────────────────────────

inline std::string hpBar(int cur, int max, int width = 10) {
    if (max <= 0) {
        std::string s;
        for (int i = 0; i < width; ++i) s += "░";
        return "[" + s + "]";
    }
    int filled = cur > 0 ? (cur * width) / max : 0;
    if (filled > width) filled = width;
    std::string bar = "[";
    for (int i = 0; i < filled; ++i) bar += "█";
    for (int i = filled; i < width; ++i) bar += "░";
    return bar + "]";
}

// ── 선 ───────────────────────────────────────────────────────────────────────

inline void line(int width = 58, char ch = '-') {
    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << "\n";
}

// ── 박스 그리기 ───────────────────────────────────────────────────────────────
// 모든 함수의 w = ║ 사이 내부 너비 (기본 58)

inline void boxTop(int w = 58) {
    std::cout << "  ╔";
    for (int i = 0; i < w; ++i) std::cout << "═";
    std::cout << "╗\n";
}
inline void boxBot(int w = 58) {
    std::cout << "  ╚";
    for (int i = 0; i < w; ++i) std::cout << "═";
    std::cout << "╝\n";
}
inline void boxMid(int w = 58) {   // ╠═══╣  굵은 구분선
    std::cout << "  ╠";
    for (int i = 0; i < w; ++i) std::cout << "═";
    std::cout << "╣\n";
}
inline void boxDiv(int w = 58) {   // ╟───╢  얇은 구분선
    std::cout << "  ╟";
    for (int i = 0; i < w; ++i) std::cout << "─";
    std::cout << "╢\n";
}
inline void boxEmpty(int w = 58) {
    std::cout << "  ║" << std::string(w, ' ') << "║\n";
}

// 왼쪽 정렬 (내부 2칸 들여쓰기)
inline void boxLeft(const std::string& s, int w = 58) {
    int pad = w - 2 - displayWidth(s);
    if (pad < 0) pad = 0;
    std::cout << "  ║  " << s << std::string(pad, ' ') << "║\n";
}

// 가운데 정렬
inline void boxCenter(const std::string& s, int w = 58) {
    int total = w - displayWidth(s);
    int left  = total / 2; if (left  < 0) left  = 0;
    int right = total - left; if (right < 0) right = 0;
    std::cout << "  ║" << std::string(left, ' ') << s
              << std::string(right, ' ') << "║\n";
}

// ── 복합 유틸 ─────────────────────────────────────────────────────────────────

inline void header(const std::string& title, int width = 58) {
    line(width, '=');
    int pad = (width - (int)title.size()) / 2;
    if (pad < 0) pad = 0;
    std::cout << "  " << std::string(pad, ' ') << title << "\n";
    line(width, '=');
}

inline void section(const std::string& label, int width = 58) {
    std::cout << "\n  " << label << "\n";
    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << '-';
    std::cout << "\n";
}

// 중앙 정렬 배너 박스
inline void banner(const std::string& line1, const std::string& line2 = "") {
    static const int W = 56;
    std::cout << "\n";
    boxTop(W);
    boxEmpty(W);
    boxCenter(line1, W);
    if (!line2.empty()) boxCenter(line2, W);
    boxEmpty(W);
    boxBot(W);
    std::cout << "\n";
}

// ── 전체 도움말 ───────────────────────────────────────────────────────────────
inline void printHelp() {
    static const int HW = 52;
    std::cout << "\n";
    boxTop(HW);
    boxCenter("[ MAP COMMANDS ]", HW);
    boxDiv(HW);
    boxLeft("w/a/s/d   Move (up/left/down/right)", HW);
    boxLeft("u/undo    Undo last move (Stack)", HW);
    boxLeft("l/look    Refresh map + party status", HW);
    boxLeft("g/graph   Dungeon graph view (DFS)", HW);
    boxLeft("i/inv     Inventory", HW);
    boxLeft("q         Stay in place", HW);
    boxDiv(HW);
    boxCenter("[ BATTLE COMMANDS ]", HW);
    boxDiv(HW);
    boxLeft("[num]     Card index -> assign to character", HW);
    boxLeft("use       Use potion from inventory", HW);
    boxLeft("switch    Swap formation positions (uses turn)", HW);
    boxLeft("l/look    Refresh battle state + hand", HW);
    boxLeft("u/undo    Undo last card assignment", HW);
    boxLeft("q         Forfeit (game over)", HW);
    boxBot(HW);
    std::cout << "\n";
}

} // namespace UI
