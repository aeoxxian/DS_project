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
    static const int HW = 56;
    std::cout << "\n";
    boxTop(HW);
    boxCenter("[ 명령어 도움말 ]", HW);
    boxDiv(HW);
    boxCenter("타이틀", HW);
    boxDiv(HW);
    boxLeft("N          새 게임 시작", HW);
    boxLeft("L          저장된 덱 불러오기", HW);
    boxDiv(HW);
    boxCenter("덱 불러오기", HW);
    boxDiv(HW);
    boxLeft("[번호]     슬롯 선택 후 파티+덱 복원", HW);
    boxLeft("d <번호>   해당 슬롯 삭제", HW);
    boxLeft("n          취소 (새 게임으로)", HW);
    boxDiv(HW);
    boxCenter("덱 구성", HW);
    boxDiv(HW);
    boxLeft("[번호]     카드 선택", HW);
    boxLeft("u/undo     마지막 선택 되돌리기", HW);
    boxLeft("save <이름>  현재 덱을 슬롯에 저장", HW);
    boxDiv(HW);
    boxCenter("맵 이동", HW);
    boxDiv(HW);
    boxLeft("w/a/s/d    이동 (위/왼쪽/아래/오른쪽)", HW);
    boxLeft("u/undo     마지막 이동 취소 (Stack)", HW);
    boxLeft("l/look     맵 + 파티 상태 새로고침", HW);
    boxLeft("g/graph    던전 그래프 보기 (DFS)", HW);
    boxLeft("i/inv      인벤토리 확인", HW);
    boxLeft("q          대기 (이동 없음)", HW);
    boxLeft("h/help     이 도움말 표시", HW);
    boxDiv(HW);
    boxCenter("전투 (카드 배정)", HW);
    boxDiv(HW);
    boxLeft("[번호]     해당 카드를 캐릭터에 배정", HW);
    boxLeft("use        인벤토리 포션 사용", HW);
    boxLeft("switch     포진 교체 (턴 소모)", HW);
    boxLeft("l/look     전투 상태 + 손패 새로고침", HW);
    boxLeft("u/undo     마지막 카드 배정 취소", HW);
    boxLeft("h/help     이 도움말 표시", HW);
    boxDiv(HW);
    boxCenter("상점 / 휴식", HW);
    boxDiv(HW);
    boxLeft("[번호]     아이템/옵션 선택", HW);
    boxLeft("q          상점 건너뜀", HW);
    boxBot(HW);
    std::cout << "\n";
}

} // namespace UI
