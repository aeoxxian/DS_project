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
    std::cout << "\n  [ Enter ] 계속... ";
    std::string s;
    std::getline(std::cin, s);
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
    int pad = w - 2 - (int)s.size();
    if (pad < 0) pad = 0;
    std::cout << "  ║  " << s << std::string(pad, ' ') << "║\n";
}

// 가운데 정렬 (ASCII/한글 혼용 시 근사치)
inline void boxCenter(const std::string& s, int w = 58) {
    int total = w - (int)s.size();
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
    static const int HW = 50;
    std::cout << "\n";
    boxTop(HW);
    boxCenter("[ 탐색 명령어 ]", HW);
    boxDiv(HW);
    boxLeft("w/a/s/d   이동 (위/왼쪽/아래/오른쪽)", HW);
    boxLeft("u/undo    직전 방으로 되돌리기 (Stack)", HW);
    boxLeft("l/look    맵 + 파티 상태 다시 보기", HW);
    boxLeft("g/graph   던전 그래프 구조 보기 (DFS)", HW);
    boxLeft("i/inv     인벤토리 확인", HW);
    boxLeft("q         현재 위치 유지", HW);
    boxDiv(HW);
    boxCenter("[ 전투 중 명령어 ]", HW);
    boxDiv(HW);
    boxLeft("[숫자]    카드 번호 입력 → 캐릭터에 배정", HW);
    boxLeft("use       인벤토리 포션 사용", HW);
    boxLeft("switch    자리 바꾸기 (행동 소비)", HW);
    boxLeft("l/look    전투 상황 + 손패 다시 보기", HW);
    boxLeft("u/undo    직전 배정 취소", HW);
    boxLeft("q         전투 포기 (게임 오버)", HW);
    boxBot(HW);
    std::cout << "\n";
}

} // namespace UI
