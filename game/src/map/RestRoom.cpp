#include "map/Room.h"
#include "core/UI.h"
#include <iostream>

static const int W               = 56;
static const int REST_HEAL_AMOUNT = 20;

static int safeInt(const std::string& s, int fallback) {
    if (s.empty()) return fallback;
    for (char c : s) if (c < '0' || c > '9') return fallback;
    return std::stoi(s);
}

void RestRoom::enter(BattleCharacter characters[], int count, CardPool& pool) {
    (void)pool;
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("★  휴  식  ★", W);
    UI::boxMid(W);
    UI::boxLeft("[0]  파티 전체 회복  (+20 HP)", W);
    UI::boxLeft("[1]  공격력 +1  (영구, 캐릭터 1명)", W);
    UI::boxLeft("[2]  방어력 +1  (영구, 캐릭터 1명)", W);
    UI::boxBot(W);
    std::cout << "\n  > ";

    std::string line;
    std::getline(std::cin, line);
    int choice = safeInt(line, 0);

    if (choice == 1 || choice == 2) {
        std::cout << "\n  캐릭터 목록:\n";
        for (int i = 0; i < count; ++i) {
            if (!characters[i].isAlive()) continue;
            std::cout << "  [" << i << "] " << characters[i].getName()
                      << "  ATK:" << characters[i].getAttackPower()
                      << "  DEF:" << characters[i].getDefend() << "\n";
        }
        std::cout << "  캐릭터 번호 > ";
        std::string charLine;
        std::getline(std::cin, charLine);
        int ci = safeInt(charLine, -1);
        if (ci < 0 || ci >= count || !characters[ci].isAlive()) {
            UI::typewrite("잘못된 선택입니다.", 15);
            return;
        }
        if (choice == 1) {
            characters[ci].permBoostAtk(1);
            UI::typewrite(characters[ci].getName() + " 공격력 +1 (영구)", 15);
        } else {
            characters[ci].permBoostDef(1);
            UI::typewrite(characters[ci].getName() + " 방어력 +1 (영구)", 15);
        }
    } else {
        healOption(characters, count);
    }
}

void RestRoom::healOption(BattleCharacter characters[], int count) {
    for (int i = 0; i < count; ++i) characters[i].heal(REST_HEAL_AMOUNT);
    std::cout << "\n";
    UI::typewrite("파티 회복 +" + std::to_string(REST_HEAL_AMOUNT) + " HP", 15);
    std::cout << "\n";
}

void RestRoom::upgradeOption(CardPool& pool) {
    (void)pool;
    // Replaced by permanent stat boost in enter()
}
