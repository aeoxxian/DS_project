#include "battle/Battle.h"
#include "run/Item.h"
#include "registry/CardRegistry.h"
#include "core/UI.h"
#include "ds/DynamicArray.h"
#include <iostream>
#include <string>
#include <cstdlib>

Battle::Battle(BattleCharacter partyArr[], int size, CardPool& sharedPool,
               Inventory* inventory, UnlockSave* unlockSave)
    : partySize(size), enemyCount(0), pool(sharedPool), inv(inventory), unlocks(unlockSave),
      turnNumber(0), battleOver(false), playerWon(false),
      totalDamageDealt(0), totalDamageTaken(0), pendingDrawBonus(0),
      currentActorIdx(-1) {
    for (int i = 0; i < partySize; ++i) party[i] = &partyArr[i];
    for (int i = 0; i < MAX_CHARACTERS; ++i) charDamageDealt[i] = 0;
}

void Battle::addEnemy(const Enemy& enemy) {
    if (enemyCount < MAX_ENEMIES) enemies[enemyCount++] = enemy;
}

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

// ── 공격 헬퍼 ─────────────────────────────────────────────────────────────────

int Battle::frontPartyIndex() const {
    for (int i = 0; i < partySize; ++i)
        if (party[i]->isAlive()) return i;
    return -1;
}

int Battle::calcRaw(const Effect& e, Combatant& src) const {
    int raw    = e.value + src.getAttackPower();
    int weaken = src.getStatus().getModifier("weaken");
    if (weaken > 0) raw = raw * (100 - weaken) / 100;
    return raw;
}

void Battle::dealToEnemy(int ei, int raw, EffectType t) {
    if (ei < 0 || ei >= enemyCount || !enemies[ei].isAlive()) return;
    int actual = enemies[ei].receiveDamage(raw, t);
    totalDamageDealt += actual;
    if (currentActorIdx >= 0 && currentActorIdx < partySize)
        charDamageDealt[currentActorIdx] += actual;
    std::cout << "    " << enemies[ei].getName() << " -" << actual << " HP\n";
}

void Battle::dealToAllEnemies(int raw, EffectType t) {
    for (int ei = 0; ei < enemyCount; ++ei) dealToEnemy(ei, raw, t);
}

void Battle::dealToParty(int raw, EffectType t) {
    int fi = frontPartyIndex();
    if (fi < 0) return;
    int actual = party[fi]->receiveDamage(raw, t);
    totalDamageTaken += actual;
    std::cout << "    " << party[fi]->getName() << " -" << actual << " HP\n";
}

// ── applyEffect ───────────────────────────────────────────────────────────────

void Battle::applyEffect(const Effect& e, Combatant& source, int targetIdx, bool sourceIsPlayer) {
    switch (e.type) {
        case EffectType::Attack: {
            int raw = calcRaw(e, source);
            if (sourceIsPlayer) {
                if (e.target == EffectTarget::AllEnemies) dealToAllEnemies(raw, e.type);
                else                                      dealToEnemy(targetIdx, raw, e.type);
            } else {
                if (e.target == EffectTarget::AllEnemies) {
                    for (int ci = 0; ci < partySize; ++ci) {
                        if (!party[ci]->isAlive()) continue;
                        int actual = party[ci]->receiveDamage(raw, e.type);
                        totalDamageTaken += actual;
                        std::cout << "    " << party[ci]->getName() << " -" << actual << " HP\n";
                    }
                } else {
                    dealToParty(raw, e.type);
                }
            }
            break;
        }
        case EffectType::HandScaleAttack: {
            int raw  = calcRaw(e, source);
            int hits = hand.size();
            std::cout << "    (Hand " << hits << " cards => " << hits << " hits)\n";
            for (int h = 0; h < hits; ++h) {
                if (sourceIsPlayer) dealToEnemy(firstLivingEnemy(), raw, e.type);
                else                dealToParty(raw, e.type);
            }
            break;
        }
        case EffectType::Defense: {
            int dur = e.duration > 0 ? e.duration : 1;
            if (e.target == EffectTarget::Party) {
                if (sourceIsPlayer) {
                    for (int ci = 0; ci < partySize; ++ci) {
                        if (!party[ci]->isAlive()) continue;
                        int sv = e.value + party[ci]->getDefend();
                        party[ci]->getStatus().apply("shield", sv, dur);
                        std::cout << "    " << party[ci]->getName() << " +" << sv << " shield\n";
                    }
                } else {
                    for (int ei = 0; ei < enemyCount; ++ei) {
                        if (!enemies[ei].isAlive()) continue;
                        int sv = e.value + enemies[ei].getDefend();
                        enemies[ei].getStatus().apply("shield", sv, dur);
                        std::cout << "    " << enemies[ei].getName() << " +" << sv << " shield\n";
                    }
                }
            } else {
                int sv = e.value + source.getDefend();
                source.getStatus().apply("shield", sv, dur);
                std::cout << "    " << source.getName() << " +" << sv << " shield\n";
            }
            break;
        }
        case EffectType::Buff: {
            auto applyBuff = [&](Combatant& target) {
                if (e.stat == "heal") {
                    int amount = e.value;
                    int burn = target.getStatus().getModifier("burn");
                    if (burn > 0) amount = amount * (100 - burn) / 100;
                    if (amount < 0) amount = 0;
                    target.heal(amount);
                    std::cout << "    " << target.getName() << " +" << amount << " HP\n";
                } else {
                    target.getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    " << target.getName() << " BUFF[" << e.stat << "]\n";
                }
            };
            if (e.target == EffectTarget::Party) {
                if (sourceIsPlayer) {
                    for (int ci = 0; ci < partySize; ++ci)
                        if (party[ci]->isAlive()) applyBuff(*party[ci]);
                } else {
                    for (int ei = 0; ei < enemyCount; ++ei)
                        if (enemies[ei].isAlive()) applyBuff(enemies[ei]);
                }
            } else {
                applyBuff(source);
            }
            break;
        }
        case EffectType::Debuff: {
            if (sourceIsPlayer) {
                if (e.target == EffectTarget::AllEnemies) {
                    for (int ei = 0; ei < enemyCount; ++ei) {
                        if (!enemies[ei].isAlive()) continue;
                        enemies[ei].getStatus().apply(e.stat, e.value, e.duration);
                        std::cout << "    " << enemies[ei].getName()
                                  << " 디버프[" << statDisplayName(e.stat) << " x" << e.duration << "턴]\n";
                    }
                } else if (targetIdx >= 0 && enemies[targetIdx].isAlive()) {
                    enemies[targetIdx].getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    " << enemies[targetIdx].getName()
                              << " 디버프[" << statDisplayName(e.stat) << " x" << e.duration << "턴]\n";
                }
            } else {
                for (int ci = 0; ci < partySize; ++ci) {
                    if (!party[ci]->isAlive()) continue;
                    party[ci]->getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    " << party[ci]->getName()
                              << " 디버프[" << statDisplayName(e.stat) << " x" << e.duration << "턴]\n";
                }
            }
            break;
        }
        case EffectType::Draw:
            pendingDrawBonus += e.value;
            std::cout << "    다음 턴 +" << e.value << " 드로우\n";
            break;
        case EffectType::Swap: {
            if (sourceIsPlayer) {
                std::cout << "    포진 교체 (예: 0 1) > ";
                std::string line;
                std::getline(std::cin, line);
                int a = -1, b = -1;
                if (line.size() >= 3 && line[0] >= '0' && line[2] >= '0') {
                    a = line[0] - '0';
                    b = line[2] - '0';
                }
                if (a >= 0 && a < partySize && b >= 0 && b < partySize && a != b) {
                    std::swap(party[a], party[b]);
                    std::cout << "    " << party[a]->getName()
                              << " <-> " << party[b]->getName() << " 교체 완료\n";
                } else {
                    std::cout << "    잘못된 입력, 건너뜀\n";
                }
            } else {
                if (partySize >= 2) {
                    int a = rand() % partySize;
                    int b; do { b = rand() % partySize; } while (b == a);
                    std::swap(party[a], party[b]);
                    std::cout << "    강제 교체! "
                              << party[a]->getName() << " <-> " << party[b]->getName() << "\n";
                }
            }
            break;
        }
    }
}

void Battle::drawPhase() {
    int drawn = 0, poolSize = pool.size();
    int drawCount = DRAW_PER_TURN + pendingDrawBonus;
    pendingDrawBonus = 0;
    if (poolSize == 0) return;
    for (int attempt = 0; drawn < drawCount && hand.slotSize() < MAX_HAND_SIZE
                          && attempt < poolSize * 2; ++attempt) {
        Card card;
        if (pool.getCard(rand() % poolSize, card)) { hand.addCard(card); ++drawn; }
    }
    std::cout << "  드로우: " << drawn << "장  (손패: " << hand.size() << ")\n";
}

void Battle::displayBattleState() const {
    static const int W = 60;
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("[ 턴  " + std::to_string(turnNumber) + " ]", W);
    UI::boxMid(W);

    // ── 파티 ────────────────────────────────────────────────────────────────
    UI::boxCenter("▲  파티", W);
    UI::boxDiv(W);
    int front = frontPartyIndex();
    for (int i = 0; i < partySize; ++i) {
        if (!party[i]->isAlive()) {
            UI::boxLeft("✗ 사망  " + party[i]->getName(), W);
            continue;
        }
        const Combatant& c = *party[i];
        bool isFront = (i == front);
        std::string row =
            (isFront ? "★전열  " : "  후열  ")
            + c.getName() + "  [" + trackToString(c.getTrack()) + "]"
            + "  HP " + std::to_string(c.getHP()) + "/" + std::to_string(c.getMaxHP())
            + "  " + UI::hpBar(c.getHP(), c.getMaxHP())
            + "  공" + std::to_string(c.getAttackPower())
            + " 방" + std::to_string(c.getDefend());
        std::string st = c.getStatus().toString();
        if (!st.empty()) row += "  " + st;
        UI::boxLeft(row, W);
    }

    UI::boxMid(W);

    // ── 적 ──────────────────────────────────────────────────────────────────
    UI::boxCenter("▼  적", W);
    UI::boxDiv(W);
    bool anyEnemy = false;
    for (int i = 0; i < enemyCount; ++i) {
        if (!enemies[i].isAlive()) continue;
        anyEnemy = true;
        const Enemy& e = enemies[i];
        std::string row =
            "[" + std::to_string(i) + "]  " + e.getName()
            + "  HP " + std::to_string(e.getHP()) + "/" + std::to_string(e.getMaxHP())
            + "  " + UI::hpBar(e.getHP(), e.getMaxHP())
            + "  공" + std::to_string(e.getAttackPower())
            + " 방" + std::to_string(e.getDefend());
        std::string st = e.getStatus().toString();
        if (!st.empty()) row += "  " + st;
        UI::boxLeft(row, W);
        Card intent;
        if (e.peekIntent(intent))
            UI::boxLeft("     → [" + intent.getName() + "]  " + intent.getDescription(), W);
        else
            UI::boxLeft("     → ???", W);
    }
    if (!anyEnemy) UI::boxLeft("  (적 없음)", W);
    UI::boxBot(W);
}

void Battle::printHand(int forChar) const {
    static const int W = 60;
    std::string forLabel;
    if (forChar >= 0 && forChar < partySize && party[forChar]->isAlive())
        forLabel = party[forChar]->getName()
                   + "  [" + trackToString(party[forChar]->getTrack()) + "]";

    std::cout << "\n";
    UI::boxTop(W);
    std::string hdr = "손패  " + std::to_string(hand.size()) + "장";
    if (!forLabel.empty()) hdr += "   ─   " + forLabel;
    UI::boxCenter(hdr, W);
    UI::boxDiv(W);

    for (int i = 0; i < hand.slotSize(); ++i) {
        Card c;
        if (!hand.peekCard(i, c)) continue;

        bool trackMatch = c.isTrackCard()
                          && forChar >= 0 && forChar < partySize
                          && party[forChar]->isAlive()
                          && party[forChar]->hasTrack(c.getTrack());

        // ★ [0] 또는    [0] — 시각 너비 동일하게 6 컬럼
        std::string prefix = (trackMatch ? "★ [" : "   [")
                             + std::to_string(i) + "]  ";
        std::string trackTag = c.isTrackCard()
                               ? "[" + trackToString(c.getTrack()) + "]"
                               : "[공용]";
        UI::boxLeft(prefix + c.getName() + "  " + trackTag, W);
        if (!c.getDescription().empty())
            UI::boxLeft("        " + c.getDescription(), W);
    }

    UI::boxDiv(W);
    UI::boxLeft("[번호]=배정   u=취소   use=포션   switch=교체   h=도움말", W);
    UI::boxBot(W);
}

void Battle::assignPhase() {
    assignStack.clear();

    int firstAlive = 0;
    while (firstAlive < partySize && !party[firstAlive]->isAlive()) ++firstAlive;
    printHand(firstAlive < partySize ? firstAlive : -1);

    int ci = 0;
    while (ci < partySize) {
        if (!party[ci]->isAlive()) { ++ci; continue; }
        if (hand.isEmpty()) { ++ci; continue; }

        bool isFront = (ci == frontPartyIndex());
        std::cout << "\n  " << (isFront ? "★ " : "    ")
                  << party[ci]->getName() << " > ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "h" || line == "help") {
            UI::printHelp();
            continue;
        }
        if (line == "l" || line == "look") {
            displayBattleState();
            printHand(ci);
            continue;
        }
        if (line == "u" || line == "undo") {
            AssignRecord rec;
            if (assignStack.pop(rec)) {
                hand.restoreCard(rec.slotIdx, rec.card);
                party[rec.ci]->clearAssignedCard();
                std::cout << "  되돌리기: [" << rec.card.getName() << "] ("
                          << party[rec.ci]->getName() << ")\n";
                printHand(rec.ci);
                ci = rec.ci;
            } else {
                std::cout << "  취소할 배정이 없습니다.\n";
            }
            continue;
        }

        if (line == "use") {
            if (!inv || inv->isEmpty()) {
                std::cout << "  인벤토리에 아이템이 없습니다.\n";
                continue;
            }
            inv->print();
            std::cout << "  아이템 번호 > ";
            std::getline(std::cin, line);
            int itemIdx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
            Item it;
            if (!inv->takeAt(itemIdx, it)) {
                std::cout << "  잘못된 번호.\n";
                continue;
            }
            switch (it.getType()) {
                case PotionType::Heal: {
                    std::cout << "  대상 캐릭터 번호 > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->heal(it.getValue());
                        std::cout << "  " << party[ti]->getName() << " HP +" << it.getValue() << "\n";
                    }
                    break;
                }
                case PotionType::AtkUp: {
                    std::cout << "  대상 캐릭터 번호 > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->getStatus().apply("atk_up", it.getValue(), 3);
                        std::cout << "  " << party[ti]->getName() << " 공격력 +" << it.getValue() << " (3턴)\n";
                    }
                    break;
                }
                case PotionType::Shield: {
                    std::cout << "  대상 캐릭터 번호 > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->getStatus().apply("shield", it.getValue(), 2);
                        std::cout << "  " << party[ti]->getName() << " 방어막 +" << it.getValue() << " (2턴)\n";
                    }
                    break;
                }
                case PotionType::Poison: {
                    std::cout << "  대상 적 번호 > ";
                    std::getline(std::cin, line);
                    int ei = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : firstLivingEnemy();
                    if (ei < 0 || ei >= enemyCount || !enemies[ei].isAlive()) ei = firstLivingEnemy();
                    if (ei >= 0) {
                        enemies[ei].getStatus().apply("poison", it.getValue(), 3);
                        std::cout << "  " << enemies[ei].getName() << " 독 (" << it.getValue() << "/턴, 3턴)\n";
                    }
                    break;
                }
                case PotionType::Explosive: {
                    std::cout << "  폭발! 모든 적에게 " << it.getValue() << " 데미지\n";
                    for (int ei = 0; ei < enemyCount; ++ei) {
                        if (!enemies[ei].isAlive()) continue;
                        enemies[ei].takeDamage(it.getValue());
                        std::cout << "    " << enemies[ei].getName() << " -" << it.getValue() << " HP\n";
                    }
                    checkBattleEnd();
                    break;
                }
            }
            continue;
        }

        if (line == "switch") {
            std::cout << "  교체할 포지션 두 개 (예: 0 1) > ";
            std::string swapLine;
            std::getline(std::cin, swapLine);
            int a = -1, b = -1;
            if (swapLine.size() >= 3 && swapLine[0] >= '0' && swapLine[2] >= '0') {
                a = swapLine[0] - '0';
                b = swapLine[2] - '0';
            }
            if (a >= 0 && a < partySize && b >= 0 && b < partySize && a != b) {
                std::swap(party[a], party[b]);
                std::cout << "  " << party[a]->getName() << " <-> " << party[b]->getName()
                          << " 교체 완료 (턴 소모)\n";
                assignStack.clear();
                ++ci;
            } else {
                std::cout << "  잘못된 입력.\n";
            }
            continue;
        }

        int idx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
        Card selected;
        if (hand.removeCard(idx, selected)) {
            party[ci]->assignCard(selected);
            bool match = selected.isTrackCard() && party[ci]->hasTrack(selected.getTrack());
            std::cout << "  ▶  [" << selected.getName() << "]";
            if (match) std::cout << "   ★트랙 일치!";
            std::cout << "\n";
            assignStack.push({ci, idx, selected});
            ++ci;
        } else {
            std::cout << "  잘못된 카드 번호.\n";
        }
    }
}

void Battle::resolveCard(const Card& card, Combatant& source, bool sourceIsPlayer, int targetIdx) {
    auto applyAll = [&](const Effect& eff) {
        if (sourceIsPlayer && card.getTargetScope() == TargetScope::All) {
            for (int ei = 0; ei < enemyCount; ++ei)
                if (enemies[ei].isAlive())
                    applyEffect(eff, source, ei, true);
        } else {
            applyEffect(eff, source, targetIdx, sourceIsPlayer);
        }
    };
    for (int ef = 0; ef < card.getEffectCount(); ++ef)
        applyAll(card.getEffect(ef));
    if (card.isTrackCard() && source.hasTrack(card.getTrack()))
        for (int ef = 0; ef < card.getBonusEffectCount(); ++ef)
            applyAll(card.getBonusEffect(ef));
}

void Battle::executePhase() {
    static const int W = 60;
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("─  행동 단계  ─", W);
    UI::boxBot(W);

    // 캐릭터 행동
    std::cout << "\n";
    UI::line(W, '-');
    std::cout << "  【 캐릭터 행동 】\n";
    UI::line(W, '-');
    bool anyAction = false;
    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive() || !party[ci]->hasCard()) continue;
        anyAction = true;
        if (party[ci]->getStatus().isStunned()) {
            std::cout << "\n  " << party[ci]->getName() << "  기절 — 행동 불가\n";
            party[ci]->clearAssignedCard();
            continue;
        }
        const Card& card = party[ci]->getAssignedCard();
        bool trackMatch  = card.isTrackCard() && party[ci]->hasTrack(card.getTrack());
        std::cout << "\n  " << party[ci]->getName()
                  << "  ▶  [" << card.getName() << "]";
        if (trackMatch) std::cout << "   ★트랙 일치!";
        std::cout << "\n";
        currentActorIdx = ci;
        resolveCard(card, *party[ci], true, firstLivingEnemy());
        currentActorIdx = -1;
        party[ci]->clearAssignedCard();
    }
    if (!anyAction) std::cout << "  (행동 없음)\n";

    // 적 행동
    std::cout << "\n";
    UI::line(W, '-');
    std::cout << "  【 적 행동 】\n";
    UI::line(W, '-');
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;
        if (enemies[ei].getStatus().isStunned()) {
            std::cout << "\n  " << enemies[ei].getName() << "  기절 — 행동 불가\n";
            Card dummy; enemies[ei].executeAndQueue(dummy);
            continue;
        }
        Card card;
        if (!enemies[ei].executeAndQueue(card)) continue;
        std::cout << "\n  " << enemies[ei].getName()
                  << "  ▶  [" << card.getName() << "]\n";
        resolveCard(card, enemies[ei], false, -1);
    }
    std::cout << "\n";
    UI::line(W, '-');
}

void Battle::statusTickPhase() {
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;
        StatusResult r = enemies[ei].getStatus().processTurn(enemies[ei].getName());
        if (r.poisonDamage > 0) enemies[ei].takeDamage(r.poisonDamage);
        enemies[ei].tickStatus();
    }
    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive()) continue;
        StatusResult r = party[ci]->getStatus().processTurn(party[ci]->getName());
        if (r.poisonDamage > 0) party[ci]->takeDamage(r.poisonDamage);
        party[ci]->tickStatus();
    }
}

void Battle::endTurnPhase() {
    hand.clear();
    for (int ci = 0; ci < partySize; ++ci) party[ci]->clearAssignedCard();
}

void Battle::rewardPhase() {
    CardRegistry& reg = CardRegistry::instance();
    if (reg.size() == 0) return;

    static const int W = 58;
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("전투 보상", W);
    UI::boxCenter("카드를 덱의 카드와 교체할 수 있습니다", W);
    UI::boxBot(W);

    // 공용 or 파티 트랙 일치 카드만 후보로
    DynamicArray<Card> pool_candidates;
    for (int i = 0; i < reg.size(); ++i) {
        Card c;
        if (!reg.getAt(i, c)) continue;
        if (c.getId() == 32) continue;
        if (c.getTrack() == Track::None) { pool_candidates.pushBack(c); continue; }
        for (int p = 0; p < partySize; ++p)
            if (party[p]->hasTrack(c.getTrack())) { pool_candidates.pushBack(c); break; }
    }
    if (pool_candidates.size() == 0) return;

    Card offers[BATTLE_REWARD_COUNT];
    int offered = 0;
    for (int attempt = 0; attempt < pool_candidates.size() * 2 && offered < BATTLE_REWARD_COUNT; ++attempt) {
        Card c = pool_candidates[rand() % pool_candidates.size()];
        // 중복 제외
        bool dup = false;
        for (int k = 0; k < offered; ++k) if (offers[k].getId() == c.getId()) { dup = true; break; }
        if (!dup) offers[offered++] = c;
    }

    for (int i = 0; i < offered; ++i) {
        std::cout << "\n";
        UI::boxTop(W);

        std::string header = "보상  " + std::to_string(i + 1) + " / " + std::to_string(offered);
        UI::boxCenter(header, W);
        UI::boxDiv(W);

        // Card name + track
        std::string nameStr = offers[i].getName();
        if (offers[i].isTrackCard())
            nameStr += "  [" + trackToString(offers[i].getTrack()) + "]";
        UI::boxLeft(nameStr, W);

        // Card description
        UI::boxLeft(offers[i].getDescription(), W);
        UI::boxDiv(W);

        UI::boxLeft("[r] 덱의 카드와 교체", W);
        UI::boxLeft("[s] 건너뜀", W);
        UI::boxBot(W);

        std::cout << "  > ";
        std::string line;
        std::getline(std::cin, line);
        char ch = line.empty() ? 's' : line[0];

        if (ch == 'r') {
            std::cout << "\n";
            pool.print();
            std::cout << "\n  교체할 카드 번호 > ";
            std::getline(std::cin, line);
            int idx = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? -1 : std::stoi(line);
            Card old;
            if (idx < 0 || !pool.getCard(idx, old)) {
                std::cout << "  잘못된 번호 -- 건너뜁니다.\n";
            } else {
                // 교체 후 트랙 한도 검사: 동일 트랙이면 -1 감안
                int countAfter = pool.countByTrack(offers[i].getTrack())
                                 - (old.getTrack() == offers[i].getTrack() ? 1 : 0);
                int lim = (offers[i].getTrack() == Track::None)
                          ? DECK_COMMON_LIMIT : DECK_TRACK_LIMIT;
                if (countAfter >= lim) {
                    std::cout << "  [" << trackToString(offers[i].getTrack())
                              << "] 한도 " << lim << "장 초과 -- 교체 불가\n";
                } else {
                    pool.removeCard(idx);
                    pool.addCard(offers[i]);
                    if (unlocks) { unlocks->unlock(offers[i].getName()); unlocks->save(); }
                    UI::typewrite("교체 완료.", 12);
                }
            }
        } else {
            std::cout << "  건너뜀.\n";
        }
    }
    std::cout << "\n";
}

bool Battle::run() {
    for (int i = 0; i < enemyCount; ++i) enemies[i].prepareIntent();

    while (!battleOver) {
        ++turnNumber;
        UI::clear();
        displayBattleState();
        drawPhase();
        assignPhase();
        executePhase();
        statusTickPhase();
        checkBattleEnd();
        if (!battleOver) {
            endTurnPhase();
            UI::pause();
        }
    }

    if (playerWon) {
        UI::sleep(300);
        UI::banner("★  승  리  ★", "모든 적을 물리쳤습니다!");
        UI::pause();
        rewardPhase();
    } else {
        UI::sleep(400);
        UI::banner("게  임  오  버", "파티 전원이 쓰러졌습니다...");
        UI::typewrite("조별과제가 이렇게 끝나는 건 처음이다.", 20);
        UI::pause();
    }
    return playerWon;
}

bool        Battle::isPlayerWon() const { return playerWon; }

BattleStats Battle::getStats() const {
    BattleStats s;
    if (enemyCount > 0) {
        s.label  = enemies[0].getName();
        s.isBoss = enemies[0].getMaxHP() > 200;
    }
    s.damageDealt = totalDamageDealt;
    s.damageTaken = totalDamageTaken;
    s.turns       = turnNumber;
    for (int i = 0; i < partySize; ++i) s.charDamage[i] = charDamageDealt[i];
    return s;
}
