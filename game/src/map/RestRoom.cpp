#include "map/Room.h"
#include <iostream>

static const int REST_HEAL_AMOUNT = 20;

void RestRoom::enter(BattleCharacter characters[], int count, CardPool& pool) {
    std::cout << "\n=== 휴식 ===\n";
    std::cout << "  [0] HP 회복 (+" << REST_HEAL_AMOUNT << ")\n";
    std::cout << "  [1] 카드 강화\n> ";
    std::string line; std::getline(std::cin, line);
    int choice = line.empty() ? 0 : std::stoi(line);
    if (choice == 1) upgradeOption(pool);
    else             healOption(characters, count);
}

void RestRoom::healOption(BattleCharacter characters[], int count) {
    for (int i = 0; i < count; ++i) characters[i].heal(REST_HEAL_AMOUNT);
    std::cout << "  -> 아군 전체 HP +" << REST_HEAL_AMOUNT << "\n";
}

void RestRoom::upgradeOption(CardPool& pool) {
    if (pool.isEmpty()) { std::cout << "  -> 카드풀이 비어있습니다.\n"; return; }
    pool.print();
    std::cout << "  강화할 카드 인덱스 > ";
    std::string line; std::getline(std::cin, line);
    int idx = line.empty() ? -1 : std::stoi(line);
    Card card;
    if (!pool.getCard(idx, card)) { std::cout << "  -> 유효하지 않은 인덱스.\n"; return; }
    std::cout << "  -> [" << card.getName() << "] 강화 완료. (추후 구현)\n";
}
