#include "Battle.h"
#include "ds/CardRegistry.h"
#include <iostream>
#include <string>
#include <cstdlib>

Battle::Battle(BattleCharacter partyArr[], int size, CardPool& sharedPool)
    : partySize(size), enemyCount(0), pool(sharedPool),
      turnNumber(0), battleOver(false), playerWon(false) {
    for (int i = 0; i < partySize; ++i) party[i] = &partyArr[i];
}

void Battle::addEnemy(const Enemy& enemy) {
    if (enemyCount < MAX_ENEMIES) enemies[enemyCount++] = enemy;
}

// ── Helpers ───────────────────────────────────────────────────────────────────
int Battle::firstLivingEnemy() const {
    for (int i = 0; i < enemyCount; ++i)
        if (enemies[i].isAlive()) return i;
    return -1;
}

int Battle::livingPartyCount() const {
    int n = 0;
    for (int i = 0; i < partySize; ++i)
        if (party[i]->isAlive()) ++n;
    return n;
}

bool Battle::checkBattleEnd() {
    if (firstLivingEnemy() == -1) { battleOver = true; playerWon = true;  return true; }
    if (livingPartyCount() == 0)  { battleOver = true; playerWon = false; return true; }
    return false;
}

// ── Effect resolution ─────────────────────────────────────────────────────────
void Battle::applyEffect(const CardEffect& e, BattleCharacter& source, int targetIdx) {
    switch (e.type) {
        case EffectType::Attack:
            if (targetIdx >= 0 && enemies[targetIdx].isAlive()) {
                enemies[targetIdx].takeDamage(e.value);
                std::cout << "    -> " << enemies[targetIdx].getName()
                          << " takes " << e.value << " damage\n";
            }
            break;
        case EffectType::Defense:
            source.getStatus().apply("def_shield", e.value, 1);
            std::cout << "    -> " << source.getName() << " gains " << e.value << " shield\n";
            break;
        case EffectType::Heal:
            source.heal(e.value);
            std::cout << "    -> " << source.getName() << " heals " << e.value << " HP\n";
            break;
        case EffectType::Buff:
            source.getStatus().apply(e.stat, e.value, e.duration);
            std::cout << "    -> " << source.getName() << " +" << e.stat << " for " << e.duration << "t\n";
            break;
        case EffectType::Debuff:
            if (targetIdx >= 0 && enemies[targetIdx].isAlive()) {
                enemies[targetIdx].getStatus().apply(e.stat, e.value, e.duration);
                std::cout << "    -> " << enemies[targetIdx].getName()
                          << " [" << e.stat << " " << e.value << " x" << e.duration << "t]\n";
            }
            break;
    }
}

// ── Draw Phase ────────────────────────────────────────────────────────────────
void Battle::drawPhase() {
    int drawn = 0, poolSize = pool.size();
    if (poolSize == 0) return;
    for (int attempt = 0; drawn < DRAW_PER_TURN && !hand.isFull() && attempt < poolSize * 2; ++attempt) {
        Card card;
        if (pool.getCard(rand() % poolSize, card) && hand.addCard(card)) ++drawn;
    }
    std::cout << "  (Drew " << drawn << " cards)\n";
}

// ── Display ───────────────────────────────────────────────────────────────────
void Battle::displayBattleState() const {
    std::cout << "\n============================== Turn " << turnNumber << "\n";
    std::cout << "[ Party ]\n";
    for (int i = 0; i < partySize; ++i) {
        std::cout << "  " << i << ". ";
        if (party[i]->isAlive()) party[i]->printStatus();
        else std::cout << party[i]->getName() << "  [사망]\n";
    }
    std::cout << "[ Enemies ]\n";
    for (int i = 0; i < enemyCount; ++i) {
        if (enemies[i].isAlive()) {
            std::cout << "  " << i << ". "; enemies[i].printStatus();
        }
    }
    std::cout << "==============================\n";
}

// ── Assign Phase ──────────────────────────────────────────────────────────────
void Battle::assignPhase() {
    hand.print();
    int alive = livingPartyCount();
    std::cout << "\n-- 카드 분배 (생존 캐릭터 " << alive << "명) --\n";

    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive()) continue;

        std::cout << "  [" << ci << "] " << party[ci]->getName() << " > ";
        std::string line; std::getline(std::cin, line);
        if (line.empty()) continue;

        Card selected;
        if (hand.removeCard(std::stoi(line), selected)) {
            party[ci]->assignCard(selected);
            std::cout << "    Assigned: "; selected.print(); std::cout << "\n";
        } else {
            std::cout << "    Invalid.\n";
        }
    }
}

// ── Execute Phase ─────────────────────────────────────────────────────────────
void Battle::executePhase() {
    std::cout << "\n-- Character actions --\n";
    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive() || !party[ci]->hasCard()) continue;

        const Card& card = party[ci]->getAssignedCard();
        bool trackMatch  = (card.getType() == CardType::TrackCard)
                           && party[ci]->hasTrack(card.getTrack());

        std::cout << "  " << party[ci]->getName() << " plays [" << card.getName() << "]";
        if (trackMatch) std::cout << " ** Track Match!";
        std::cout << "\n";

        int target = firstLivingEnemy();
        for (int e = 0; e < card.getEffectCount(); ++e)
            applyEffect(card.getEffect(e), *party[ci], target);
        if (trackMatch)
            for (int e = 0; e < card.getBonusEffectCount(); ++e)
                applyEffect(card.getBonusEffect(e), *party[ci], target);

        party[ci]->clearAssignedCard();
    }

    std::cout << "\n-- Enemy actions --\n";
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;

        if (enemies[ei].getStatus().isStunned()) {
            std::cout << "  " << enemies[ei].getName() << " is stunned!\n";
            enemies[ei].decideIntent();
            continue;
        }

        int dmg = enemies[ei].executeIntent();
        if (dmg > 0) {
            for (int ci = 0; ci < partySize; ++ci) {
                if (!party[ci]->isAlive()) continue;
                int shield = party[ci]->getStatus().getModifier("def_shield");
                int taken  = dmg - shield;
                if (taken > 0) party[ci]->takeDamage(taken);
            }
        }
        enemies[ei].decideIntent();
    }
}

// ── Status Tick ───────────────────────────────────────────────────────────────
void Battle::statusTickPhase() {
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;
        int poison = enemies[ei].getStatus().getModifier("poison");
        if (poison > 0) {
            enemies[ei].takeDamage(poison);
            std::cout << "  " << enemies[ei].getName() << " takes " << poison << " poison\n";
        }
        enemies[ei].tickStatus();
    }
    for (int ci = 0; ci < partySize; ++ci)
        if (party[ci]->isAlive()) party[ci]->tickStatus();
}

// ── End Turn ──────────────────────────────────────────────────────────────────
void Battle::endTurnPhase() {
    hand.clear();
    for (int ci = 0; ci < partySize; ++ci) party[ci]->clearAssignedCard();
}

// ── Reward Phase ──────────────────────────────────────────────────────────────
void Battle::rewardPhase() {
    CardRegistry& reg = CardRegistry::instance();
    if (reg.size() == 0) return;

    std::cout << "\n=== Battle Reward ===\n";
    Card offers[BATTLE_REWARD_COUNT];
    int offered = 0;
    for (int attempt = 0; attempt < reg.size() * 2 && offered < BATTLE_REWARD_COUNT; ++attempt) {
        Card c;
        if (reg.getAt(rand() % reg.size(), c)) offers[offered++] = c;
    }

    for (int i = 0; i < offered; ++i) {
        std::cout << "\n  Offer " << (i+1) << ": "; offers[i].print(); std::cout << "\n";
        std::cout << "  [a]Add  [r]Replace  [s]Skip > ";
        std::string line; std::getline(std::cin, line);
        char ch = line.empty() ? 's' : line[0];

        if (ch == 'a') {
            pool.addCard(offers[i]) ? std::cout << "  -> Added.\n"
                                    : std::cout << "  -> Pool full.\n";
        } else if (ch == 'r') {
            pool.print();
            std::cout << "  Replace index > ";
            std::getline(std::cin, line);
            int idx = line.empty() ? -1 : std::stoi(line);
            if (pool.removeCard(idx)) { pool.addCard(offers[i]); std::cout << "  -> Replaced.\n"; }
            else std::cout << "  -> Invalid.\n";
        }
    }
}

// ── Main Loop ─────────────────────────────────────────────────────────────────
bool Battle::run() {
    for (int i = 0; i < enemyCount; ++i) enemies[i].decideIntent();

    while (!battleOver) {
        ++turnNumber;
        displayBattleState();
        drawPhase();
        assignPhase();
        executePhase();
        statusTickPhase();
        checkBattleEnd();
        if (!battleOver) endTurnPhase();
    }

    if (playerWon) { std::cout << "\n=== Victory! ===\n"; rewardPhase(); }
    else             std::cout << "\n=== Defeated. ===\n";

    return playerWon;
}

bool Battle::isPlayerWon() const { return playerWon; }
