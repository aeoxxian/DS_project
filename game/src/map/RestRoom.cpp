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
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("★  휴식  ★", W);
    UI::boxMid(W);
    UI::boxLeft("[0]  HP 회복 (+" + std::to_string(REST_HEAL_AMOUNT) + ")", W);
    UI::boxLeft("[1]  카드 강화", W);
    UI::boxBot(W);
    std::cout << "\n  선택 > ";

    std::string line;
    std::getline(std::cin, line);
    int choice = safeInt(line, 0);

    if (choice == 1) upgradeOption(pool);
    else             healOption(characters, count);
}

void RestRoom::healOption(BattleCharacter characters[], int count) {
    for (int i = 0; i < count; ++i) characters[i].heal(REST_HEAL_AMOUNT);
    std::cout << "\n";
    UI::typewrite("아군 전체 HP +" + std::to_string(REST_HEAL_AMOUNT), 15);
    std::cout << "\n";
}

void RestRoom::upgradeOption(CardPool& pool) {
    if (pool.isEmpty()) {
        UI::typewrite("카드풀이 비어있습니다.", 15);
        return;
    }
    pool.print();
    std::cout << "\n  강화할 카드 인덱스 > ";
    std::string line;
    std::getline(std::cin, line);
    int idx = safeInt(line, -1);
    Card card;
    if (!pool.getCard(idx, card)) {
        UI::typewrite("유효하지 않은 인덱스.", 15);
        return;
    }
    UI::typewrite("[" + card.getName() + "] 강화 완료. (추후 구현)", 15);
}
