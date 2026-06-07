#include "registry/Registries.h"
#include "run/Game.h"
#include "core/UI.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    registerAll();

    while (true) {
        UI::clear();
        std::cout << "\n";
        std::cout << "  ╔════════════════════════════════════════════╗\n";
        std::cout << "  ║    D U N G E O N   E X P L O R E R        ║\n";
        std::cout << "  ║    C++ 자료구조 PBL 프로젝트               ║\n";
        std::cout << "  ╠════════════════════════════════════════════╣\n";
        std::cout << "  ║                                            ║\n";
        std::cout << "  ║          [1]  새 게임 시작                  ║\n";
        std::cout << "  ║          [0]  종료                          ║\n";
        std::cout << "  ║                                            ║\n";
        std::cout << "  ╚════════════════════════════════════════════╝\n\n";
        std::cout << "  > ";

        std::string line;
        std::getline(std::cin, line);
        int choice = (!line.empty() && line[0] >= '0' && line[0] <= '9')
                     ? (line[0] - '0') : -1;

        if (choice == 0) break;
        if (choice == 1) {
            Game game;
            game.run();
            UI::pause();
        }
    }

    UI::clear();
    std::cout << "\n  게임을 종료합니다.\n\n";
    return 0;
}
