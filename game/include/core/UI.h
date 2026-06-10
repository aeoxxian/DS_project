#pragma once
#include <iostream>
#include <string>
#include <cstdlib>

namespace UI {

inline void clear() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

inline void pause() {
    std::cout << "\n  [Enter] 계속...";
    std::string s;
    std::getline(std::cin, s);
}

inline std::string hpBar(int cur, int max, int width = 10) {
    if (max <= 0) {
        std::string s;
        for (int i = 0; i < width; ++i) s += "░";
        return "[" + s + "]";
    }
    int filled = cur > 0 ? (cur * width) / max : 0;
    if (filled > width) filled = width;
    std::string bar = "[";
    for (int i = 0; i < filled;        ++i) bar += "█";
    for (int i = filled; i < width;    ++i) bar += "░";
    return bar + "]";
}

inline void line(int width = 58, char ch = '-') {
    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << "\n";
}

// 굵은 구분선 + 제목
inline void header(const std::string& title, int width = 58) {
    line(width, '=');
    // 가운데 정렬
    int pad = (width - (int)title.size()) / 2;
    if (pad < 0) pad = 0;
    std::cout << "  " << std::string(pad, ' ') << title << "\n";
    line(width, '=');
}

// 얇은 구분선 + 소제목
inline void section(const std::string& label, int width = 58) {
    std::cout << "\n  " << label << "\n";
    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << '-';
    std::cout << "\n";
}

// 강조 배너 (방 입장 등)
inline void banner(const std::string& line1, const std::string& line2 = "") {
    static const int W = 46;
    std::cout << "\n  ";
    for (int i = 0; i < W; ++i) std::cout << "═";
    std::cout << "\n  " << line1 << "\n";
    if (!line2.empty()) std::cout << "  " << line2 << "\n";
    std::cout << "  ";
    for (int i = 0; i < W; ++i) std::cout << "═";
    std::cout << "\n\n";
}

} // namespace UI
