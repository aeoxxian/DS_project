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
    std::cout << "\n  [ Enter ]  계속... ";
    std::string s;
    std::getline(std::cin, s);
}

// UTF-8 display width: Korean/CJK = 2 cols, ASCII/box-drawing/block-elements = 1 col
inline int displayWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = (unsigned char)s[i];
        if (c < 0x80) { i += 1; w += 1; }
        else if (c < 0xE0) { i += 2; w += 1; }
        else if (c < 0xF0) {
            unsigned char b2 = (i+1 < s.size()) ? (unsigned char)s[i+1] : 0;
            // Narrow 1-cell sequences within E2 prefix:
            //   E2 94/95 = box-drawing (─║╔╗…)
            //   E2 96 80-9F = block elements (█░▒▓)
            //   E2 9C-9E = dingbats (✗✓…)
            bool narrow = (c == 0xE2) && (
                b2 == 0x94 || b2 == 0x95
                || (b2 == 0x96 && i+2 < s.size() && (unsigned char)s[i+2] <= 0x9F)
                || b2 == 0x9C || b2 == 0x9D || b2 == 0x9E
            );
            i += 3; w += (narrow ? 1 : 2);
        }
        else { i += 4; w += 2; }
    }
    return w;
}

// Truncate s so displayWidth <= maxW, appending "..." if cut
inline std::string truncToWidth(const std::string& s, int maxW) {
    if (maxW <= 3 || displayWidth(s) <= maxW) return s;
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = (unsigned char)s[i];
        int cl; int cw;
        if (c < 0x80)      { cl = 1; cw = 1; }
        else if (c < 0xE0) { cl = 2; cw = 1; }
        else if (c < 0xF0) {
            cl = 3;
            unsigned char b2 = (i+1 < s.size()) ? (unsigned char)s[i+1] : 0;
            bool narrow = (c == 0xE2) && (
                b2 == 0x94 || b2 == 0x95
                || (b2 == 0x96 && i+2 < s.size() && (unsigned char)s[i+2] <= 0x9F)
                || b2 == 0x9C || b2 == 0x9D || b2 == 0x9E
            );
            cw = narrow ? 1 : 2;
        }
        else { cl = 4; cw = 2; }
        if (w + cw > maxW - 3) return s.substr(0, i) + "...";
        w += cw; i += cl;
    }
    return s;
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

// 왼쪽 정렬 — 넘치면 "..." 자동 절단
inline void boxLeftTrunc(const std::string& s, int w = 58) {
    boxLeft(truncToWidth(s, w - 2), w);
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
    static const int HW = 54;
    std::cout << "\n";
    boxTop(HW);
    boxCenter("명령어 도움말", HW);
    boxMid(HW);

    boxCenter("게임 시작", HW);
    boxDiv(HW);
    boxLeft("Y / N        저장 불러오기 / 새 게임 (기본 덱)", HW);
    boxLeft("[0]          불러온 후 새 덱 구성 (해금 카드)", HW);
    boxLeft("[번호]       저장된 프리셋 불러오기", HW);

    boxMid(HW);
    boxCenter("덱 구성  ─  카드 선택 단계", HW);
    boxDiv(HW);
    boxLeft("[번호]       카드 선택", HW);
    boxLeft("u / undo     마지막 선택 취소", HW);
    boxLeft("save <이름>  현재 덱을 슬롯에 저장", HW);

    boxMid(HW);
    boxCenter("맵 이동", HW);
    boxDiv(HW);
    boxLeft("w/a/s/d          방향 이동", HW);
    boxLeft("u / undo         마지막 이동 취소", HW);
    boxLeft("l / look         맵 + 파티 상태 새로고침", HW);
    boxLeft("g / graph        던전 그래프 (DFS)", HW);
    boxLeft("i / inv          인벤토리 확인", HW);
    boxLeft("save <이름>      현재 덱·파티 구성 저장", HW);
    boxLeft("h / help         도움말", HW);

    boxMid(HW);
    boxCenter("전투  ─  카드 배정", HW);
    boxDiv(HW);
    boxLeft("[번호]       카드를 현재 캐릭터에 배정", HW);
    boxLeft("u / undo     마지막 배정 취소", HW);
    boxLeft("use          포션 사용", HW);
    boxLeft("switch       포진 교체 (전열↔후열, 턴 소모)", HW);
    boxLeft("l / look     전투 상태 + 손패 새로고침", HW);
    boxLeft("★            카드 앞 별표 = 현재 캐릭터 트랙 일치", HW);

    boxMid(HW);
    boxCenter("전투 보상  ─  카드 교체", HW);
    boxDiv(HW);
    boxLeft("r            덱의 카드와 교체 (25장 유지)", HW);
    boxLeft("s            건너뜀", HW);

    boxMid(HW);
    boxCenter("상점 / 휴식", HW);
    boxDiv(HW);
    boxLeft("[번호]       아이템 선택", HW);
    boxLeft("q            건너뜀", HW);

    boxBot(HW);
    std::cout << "\n";
}

} // namespace UI
