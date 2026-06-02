#include "Battle.h"
#include "ds/CardRegistry.h"
#include <iostream>
#include <string>
#include <cstdlib>

Battle::Battle(BattleCharacter partyArr[], int size, CardPool& sharedPool)
    : partySize(size), enemyCount(0), pool(sharedPool),
      turnNumber(0), battleOver(false), playerWon(false), pendingDraw(0) {
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

// ── Damage pipeline ─────────────────────────────────────────────────────────
// 출력 데미지: (base + atk_up - atk_down) 후 weaken% 감소.
int Battle::outgoingDamage(int base, StatusTracker& src) const {
    int out = base + src.getModifier(ST_ATK_UP) - src.getModifier(ST_ATK_DOWN);
    int weaken = src.getModifier(ST_WEAKEN);
    if (weaken > 0) out = out * (100 - weaken) / 100;
    return out < 0 ? 0 : out;
}

// 피격 데미지: block(1회 소모) -> evade(확률 회피) -> shield 흡수(소모) -> def_up/def_down 보정.
int Battle::applyIncoming(int dmg, StatusTracker& tgt) const {
    if (dmg <= 0) return 0;
    if (tgt.has(ST_BLOCK)) { tgt.remove(ST_BLOCK); return 0; }
    int evade = tgt.getModifier(ST_EVADE);
    if (evade > 0 && (rand() % 100) < evade) return 0;
    int shield = tgt.getModifier(ST_SHIELD);
    if (shield > 0) {
        int absorbed = (shield < dmg) ? shield : dmg;
        tgt.consume(ST_SHIELD, absorbed);
        dmg -= absorbed;
    }
    dmg = dmg - tgt.getModifier(ST_DEF_UP) + tgt.getModifier(ST_DEF_DOWN);
    return dmg < 0 ? 0 : dmg;
}

void Battle::dealToEnemy(int enemyIdx, int raw, StatusTracker& src) {
    if (enemyIdx < 0 || enemyIdx >= enemyCount || !enemies[enemyIdx].isAlive()) return;
    int out = outgoingDamage(raw, src);
    int dmg = applyIncoming(out, enemies[enemyIdx].getStatus());
    enemies[enemyIdx].takeDamage(dmg);
    std::cout << "    -> " << enemies[enemyIdx].getName() << " takes " << dmg << " damage";
    if (dmg == 0) std::cout << " (blocked/evaded)";
    std::cout << "\n";
}

// ── Effect resolution ─────────────────────────────────────────────────────────
// 효과 대상(EffectTarget)에 따라 시전자/파티/적/적 전체로 분기.
void Battle::applyEffect(const CardEffect& e, BattleCharacter& source, int targetIdx) {
    switch (e.type) {
        case EffectType::Attack:
            if (e.target == EffectTarget::AllEnemies) {
                for (int i = 0; i < enemyCount; ++i) dealToEnemy(i, e.value, source.getStatus());
            } else {
                dealToEnemy(targetIdx, e.value, source.getStatus());
            }
            break;

        case EffectType::Debuff: {
            bool all = (e.target == EffectTarget::AllEnemies);
            for (int i = 0; i < enemyCount; ++i) {
                if (!all && i != targetIdx) continue;
                if (i < 0 || !enemies[i].isAlive()) continue;
                enemies[i].getStatus().apply(e.stat, e.value, e.duration);
                std::cout << "    -> " << enemies[i].getName()
                          << " [" << e.stat << " " << e.value << " x" << e.duration << "t]\n";
                if (!all) break;
            }
            break;
        }

        case EffectType::Defense: {
            int dur = (e.duration > 0) ? e.duration : 1;
            if (e.target == EffectTarget::Party) {
                for (int i = 0; i < partySize; ++i) {
                    if (!party[i]->isAlive()) continue;
                    party[i]->getStatus().apply(ST_SHIELD, e.value, dur);
                    std::cout << "    -> " << party[i]->getName() << " gains " << e.value << " shield\n";
                }
            } else {
                source.getStatus().apply(ST_SHIELD, e.value, dur);
                std::cout << "    -> " << source.getName() << " gains " << e.value << " shield\n";
            }
            break;
        }

        case EffectType::Heal:
            if (e.target == EffectTarget::Party) {
                for (int i = 0; i < partySize; ++i) {
                    if (!party[i]->isAlive()) continue;
                    int amt = e.value - party[i]->getStatus().getModifier(ST_BURN);
                    if (amt < 0) amt = 0;
                    party[i]->heal(amt);
                    std::cout << "    -> " << party[i]->getName() << " heals " << amt << " HP\n";
                }
            } else {
                int amt = e.value - source.getStatus().getModifier(ST_BURN);
                if (amt < 0) amt = 0;
                source.heal(amt);
                std::cout << "    -> " << source.getName() << " heals " << amt << " HP\n";
            }
            break;

        case EffectType::Buff:
            if (e.target == EffectTarget::Party) {
                for (int i = 0; i < partySize; ++i) {
                    if (!party[i]->isAlive()) continue;
                    party[i]->getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    -> " << party[i]->getName() << " +" << e.stat
                              << " for " << e.duration << "t\n";
                }
            } else {
                source.getStatus().apply(e.stat, e.value, e.duration);
                std::cout << "    -> " << source.getName() << " +" << e.stat
                          << " for " << e.duration << "t\n";
            }
            break;

        case EffectType::Draw:
            // 즉시 뽑지 않고 "다음 턴" 드로우 수를 늘린다 (1턴 지속). 손패 상한(10장)은 drawPhase에서 처리.
            pendingDraw += e.value;
            std::cout << "    -> " << source.getName() << " : 다음 턴 +" << e.value << " 드로우\n";
            break;
    }
}

// ── Draw ──────────────────────────────────────────────────────────────────────
// 풀에서 최대 n장을 손패에 추가(랜덤). 손패가 10장(MAX_HAND_SIZE)이면 멈춤. 실제 뽑은 수 반환.
int Battle::drawCards(int n) {
    int poolSize = pool.size();
    if (poolSize == 0 || n <= 0) return 0;
    int drawn = 0;
    for (int attempt = 0; drawn < n && !hand.isFull() && attempt < poolSize * 2 + n; ++attempt) {
        Card card;
        if (pool.getCard(rand() % poolSize, card) && hand.addCard(card)) ++drawn;
    }
    return drawn;
}

void Battle::drawPhase() {
    // 매턴 기본 5장 + 이전 턴에 적립된 보너스. 보너스는 1턴만 유지되므로 사용 후 리셋.
    int target = DRAW_PER_TURN + pendingDraw;
    if (pendingDraw > 0)
        std::cout << "  (드로우 보너스 +" << pendingDraw << ")\n";
    pendingDraw = 0;
    int drawn = drawCards(target);
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

        // ── 공유 지점(전투 흐름) ─────────────────────────────────────────────
        // 적 공격이 아군 방어 상태(shield/def_up/evade/block)를 실제로 읽도록 applyIncoming 경유.
        // 타겟 정책(파티 전원 동시 피격)은 기존 그대로 유지 — 변경 시 전투 흐름 담당자와 협의.
        int dmg = enemies[ei].executeIntent();      // base + atk_up - atk_down (Enemy::getAttackPower)
        if (dmg > 0) {
            int weaken = enemies[ei].getStatus().getModifier(ST_WEAKEN);
            if (weaken > 0) dmg = dmg * (100 - weaken) / 100;   // weaken%는 여기서 적용

            int confuse = enemies[ei].getStatus().getModifier(ST_CONFUSE);
            if (confuse > 0 && (rand() % 100) < confuse) {
                enemies[ei].takeDamage(dmg);
                std::cout << "  " << enemies[ei].getName()
                          << " is confused and hits itself for " << dmg << "!\n";
            } else {
                for (int ci = 0; ci < partySize; ++ci) {
                    if (!party[ci]->isAlive()) continue;
                    int taken = applyIncoming(dmg, party[ci]->getStatus());
                    if (taken > 0) party[ci]->takeDamage(taken);
                }
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
    // 손패는 매 턴 리셋 — 남은 카드는 버리고 다음 턴에 새로 드로우.
    // (Draw 카드의 효과는 pendingDraw로 다음 턴 드로우 수에만 반영됨)
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
