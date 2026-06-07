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

// HP 바: 예) ████████░░  (filled=█, empty=░)
inline std::string hpBar(int cur, int max, int width = 8) {
    if (max <= 0) {
        std::string s;
        for (int i = 0; i < width; ++i) s += "░";
        return s;
    }
    int filled = cur > 0 ? (cur * width) / max : 0;
    if (filled > width) filled = width;
    std::string bar;
    for (int i = 0; i < filled;  ++i) bar += "█";
    for (int i = filled; i < width; ++i) bar += "░";
    return bar;
}

inline void line(int width = 54, char ch = '-') {
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << "\n";
}

} // namespace UI
