#include "battle/Battle.h"
#include "run/Item.h"
#include "registry/CardRegistry.h"
#include "core/UI.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>

Battle::Battle(BattleCharacter partyArr[], int size, CardPool& sharedPool, Inventory* inventory)
    : partySize(size), enemyCount(0), pool(sharedPool), inv(inventory),
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
    std::cout << "    -> " << enemies[ei].getName() << " -" << actual << " HP\n";
}

void Battle::dealToAllEnemies(int raw, EffectType t) {
    for (int ei = 0; ei < enemyCount; ++ei) dealToEnemy(ei, raw, t);
}

void Battle::dealToParty(int raw, EffectType t) {
    int fi = frontPartyIndex();
    if (fi < 0) return;
    int actual = party[fi]->receiveDamage(raw, t);
    totalDamageTaken += actual;
    std::cout << "    -> " << party[fi]->getName() << " -" << actual << " HP\n";
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
                        std::cout << "    -> " << party[ci]->getName() << " -" << actual << " HP\n";
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
            std::cout << "    (손패 " << hits << "장 → " << hits << "회 공격)\n";
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
                        std::cout << "    -> " << party[ci]->getName() << " +" << sv << " shield\n";
                    }
                } else {
                    for (int ei = 0; ei < enemyCount; ++ei) {
                        if (!enemies[ei].isAlive()) continue;
                        int sv = e.value + enemies[ei].getDefend();
                        enemies[ei].getStatus().apply("shield", sv, dur);
                        std::cout << "    -> " << enemies[ei].getName() << " +" << sv << " shield\n";
                    }
                }
            } else {
                int sv = e.value + source.getDefend();
                source.getStatus().apply("shield", sv, dur);
                std::cout << "    -> " << source.getName() << " +" << sv << " shield\n";
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
                    std::cout << "    -> " << target.getName() << " +" << amount << " HP\n";
                } else {
                    target.getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    -> " << target.getName() << " BUFF[" << e.stat << "]\n";
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
                        std::cout << "    -> " << enemies[ei].getName()
                                  << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                    }
                } else if (targetIdx >= 0 && enemies[targetIdx].isAlive()) {
                    enemies[targetIdx].getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    -> " << enemies[targetIdx].getName()
                              << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                }
            } else {
                for (int ci = 0; ci < partySize; ++ci) {
                    if (!party[ci]->isAlive()) continue;
                    party[ci]->getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    -> " << party[ci]->getName()
                              << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                }
            }
            break;
        }
        case EffectType::Draw:
            pendingDrawBonus += e.value;
            std::cout << "    -> 다음 턴 +" << e.value << "장 드로우\n";
            break;
        case EffectType::Swap: {
            if (sourceIsPlayer) {
                std::cout << "    자리 바꾸기 (예: 0 1) > ";
                std::string line;
                std::getline(std::cin, line);
                int a = -1, b = -1;
                if (line.size() >= 3 && line[0] >= '0' && line[2] >= '0') {
                    a = line[0] - '0';
                    b = line[2] - '0';
                }
                if (a >= 0 && a < partySize && b >= 0 && b < partySize && a != b) {
                    std::swap(party[a], party[b]);
                    std::cout << "    -> " << party[a]->getName()
                              << " ↔ " << party[b]->getName() << " 자리 교환\n";
                } else {
                    std::cout << "    -> 잘못된 입력, 스킵\n";
                }
            } else {
                if (partySize >= 2) {
                    int a = rand() % partySize;
                    int b; do { b = rand() % partySize; } while (b == a);
                    std::swap(party[a], party[b]);
                    std::cout << "    -> 강제 자리 바꾸기! "
                              << party[a]->getName() << " ↔ " << party[b]->getName() << "\n";
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
    for (int attempt = 0; drawn < drawCount && hand.size() < MAX_HAND_SIZE
                          && attempt < poolSize * 2; ++attempt) {
        Card card;
        if (pool.getCard(rand() % poolSize, card)) { hand.addCard(card); ++drawn; }
    }
    std::cout << "  드로우: " << drawn << "장  (손패 " << hand.size() << "장)\n";
}

void Battle::displayBattleState() const {
    static const int W = 58;
    std::cout << "\n";
    UI::line(W, '=');
    std::cout << "  TURN " << turnNumber << "\n";
    UI::line(W, '=');

    // ── 파티 ────────────────────────────────────────────────────────────────
    UI::section("파티", W);
    int front = frontPartyIndex();
    for (int i = 0; i < partySize; ++i) {
        if (!party[i]->isAlive()) {
            std::cout << "  [" << i << "] ------  "
                      << std::left << std::setw(8) << party[i]->getName()
                      << "  사망\n";
            continue;
        }
        const Combatant& c = *party[i];
        bool isFront = (i == front);
        std::cout << "  " << (isFront ? "★전열" : "  후열")
                  << "  " << std::left << std::setw(8) << c.getName()
                  << " [" << std::setw(11) << trackToString(c.getTrack()) << "]"
                  << "  HP " << std::right << std::setw(3) << c.getHP()
                  << "/" << std::setw(3) << c.getMaxHP()
                  << " " << UI::hpBar(c.getHP(), c.getMaxHP())
                  << "  ATK:" << std::setw(2) << c.getAttackPower()
                  << " DEF:" << std::setw(2) << c.getDefend();
        std::string st = c.getStatus().toString();
        if (!st.empty()) std::cout << " " << st;
        std::cout << "\n";
    }

    // ── 적 ──────────────────────────────────────────────────────────────────
    UI::section("적", W);
    for (int i = 0; i < enemyCount; ++i) {
        if (!enemies[i].isAlive()) continue;
        const Enemy& e = enemies[i];
        Card intent;
        std::string intentName = e.peekIntent(intent) ? intent.getName() : "???";
        std::cout << "  [" << i << "] "
                  << std::left << std::setw(16) << e.getName()
                  << " HP " << std::right << std::setw(3) << e.getHP()
                  << "/" << std::setw(3) << e.getMaxHP()
                  << " " << UI::hpBar(e.getHP(), e.getMaxHP())
                  << "  ATK:" << std::setw(2) << e.getAttackPower()
                  << " DEF:" << std::setw(2) << e.getDefend();
        std::string st = e.getStatus().toString();
        if (!st.empty()) std::cout << " " << st;
        std::cout << "   다음: " << intentName << "\n";
    }

    std::cout << "\n";
    UI::line(W, '=');
}

void Battle::printHand() const {
    static const int W = 58;
    UI::section("손패  (" + std::to_string(hand.size()) + "장)", W);

    for (int i = 0; i < hand.size(); ++i) {
        Card c;
        if (!hand.peekCard(i, c)) continue;

        bool trackMatch = false;
        if (c.isTrackCard())
            for (int ci = 0; ci < partySize; ++ci)
                if (party[ci]->isAlive() && party[ci]->hasTrack(c.getTrack()))
                    { trackMatch = true; break; }

        std::cout << "  [" << i << "] "
                  << std::left << std::setw(18) << c.getName();

        if (c.isTrackCard())
            std::cout << " [" << std::setw(11) << trackToString(c.getTrack())
                      << (trackMatch ? "★]" : " ]");
        else
            std::cout << "              ";

        for (int e = 0; e < c.getEffectCount(); ++e)
            std::cout << "  " << c.getEffect(e).toString();
        if (c.getBonusEffectCount() > 0) {
            std::cout << "  +[";
            for (int e = 0; e < c.getBonusEffectCount(); ++e)
                std::cout << c.getBonusEffect(e).toString();
            std::cout << "]";
        }
        if (c.getTargetScope() == TargetScope::All) std::cout << "  전체";
        std::cout << "\n";
    }
}

void Battle::assignPhase() {
    static const int W = 58;
    assignStack.clear();
    printHand();

    std::cout << "\n";
    UI::line(W, '-');
    std::cout << "  카드 배정   생존 " << livingPartyCount() << "명";
    std::cout << "   h=도움말  u=되돌리기  switch=자리바꾸기\n";
    UI::line(W, '-');
    std::cout << "\n";

    int ci = 0;
    while (ci < partySize) {
        if (!party[ci]->isAlive()) { ++ci; continue; }
        if (hand.isEmpty()) { ++ci; continue; }  // 손패 소진 시 자동 스킵

        bool isFront = (ci == frontPartyIndex());
        std::cout << "  " << (isFront ? "★" : "  ")
                  << party[ci]->getName() << " > ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "h" || line == "help") {
            std::cout << "  카드 번호 입력 → 해당 캐릭터에 배정 (필수)\n";
            std::cout << "  switch         → 자리 바꾸기 (카드 대신, 이 캐릭터 행동 소비)\n";
            std::cout << "  use            → 인벤토리 아이템 사용 (포션 등)\n";
            std::cout << "  l / look       → 전투 상황 + 손패 다시 보기\n";
            std::cout << "  u / undo       → 직전 배정 취소 후 해당 캐릭터로 돌아가기\n";
            continue;
        }
        if (line == "l" || line == "look") {
            displayBattleState();
            printHand();
            std::cout << "\n";
            UI::line(56, '-');
            continue;
        }
        if (line == "u" || line == "undo") {
            AssignRecord rec;
            if (assignStack.pop(rec)) {
                hand.addCard(rec.card);
                party[rec.ci]->clearAssignedCard();
                std::cout << "  ← [" << rec.card.getName() << "] 배정 취소 ("
                          << party[rec.ci]->getName() << ")\n";
                printHand();
                std::cout << "\n";
                UI::line(56, '-');
                ci = rec.ci;
            } else {
                std::cout << "  더 이상 되돌릴 수 없습니다.\n";
            }
            continue;
        }

        if (line == "use") {
            if (!inv || inv->isEmpty()) {
                std::cout << "  사용할 아이템이 없습니다.\n";
                continue;
            }
            inv->print();
            std::cout << "  사용할 아이템 번호 > ";
            std::getline(std::cin, line);
            int itemIdx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
            Item it;
            if (!inv->takeAt(itemIdx, it)) {
                std::cout << "  잘못된 번호.\n";
                continue;
            }
            switch (it.getType()) {
                case PotionType::Heal: {
                    std::cout << "  사용 대상 캐릭터 번호 > ";
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
                    std::cout << "  사용 대상 캐릭터 번호 > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->getStatus().apply("atk_up", it.getValue(), 3);
                        std::cout << "  " << party[ti]->getName() << " ATK +" << it.getValue() << " (3턴)\n";
                    }
                    break;
                }
                case PotionType::Shield: {
                    std::cout << "  사용 대상 캐릭터 번호 > ";
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
                    std::cout << "  독을 부여할 적 번호 > ";
                    std::getline(std::cin, line);
                    int ei = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : firstLivingEnemy();
                    if (ei < 0 || ei >= enemyCount || !enemies[ei].isAlive()) ei = firstLivingEnemy();
                    if (ei >= 0) {
                        enemies[ei].getStatus().apply("poison", it.getValue(), 3);
                        std::cout << "  " << enemies[ei].getName() << " 에 독 부여 (" << it.getValue() << "/턴, 3턴)\n";
                    }
                    break;
                }
                case PotionType::Explosive: {
                    std::cout << "  폭발! 전체 적에게 " << it.getValue() << " 피해\n";
                    for (int ei = 0; ei < enemyCount; ++ei) {
                        if (!enemies[ei].isAlive()) continue;
                        enemies[ei].takeDamage(it.getValue());
                        std::cout << "    -> " << enemies[ei].getName() << " -" << it.getValue() << " HP\n";
                    }
                    checkBattleEnd();
                    break;
                }
            }
            continue;
        }

        if (line == "switch") {
            std::cout << "  자리 바꿀 두 번호 입력 (예: 0 1) > ";
            std::string swapLine;
            std::getline(std::cin, swapLine);
            int a = -1, b = -1;
            if (swapLine.size() >= 3 && swapLine[0] >= '0' && swapLine[2] >= '0') {
                a = swapLine[0] - '0';
                b = swapLine[2] - '0';
            }
            if (a >= 0 && a < partySize && b >= 0 && b < partySize && a != b) {
                std::swap(party[a], party[b]);
                std::cout << "  -> " << party[a]->getName() << " ↔ " << party[b]->getName()
                          << " 자리 교환 (행동 소비)\n";
                assignStack.clear();
                ++ci;
            } else {
                std::cout << "  -> 잘못된 번호.\n";
            }
            continue;
        }

        int idx = (line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
        Card selected;
        if (hand.removeCard(idx, selected)) {
            party[ci]->assignCard(selected);
            bool match = selected.isTrackCard() && party[ci]->hasTrack(selected.getTrack());
            std::cout << "     → [" << selected.getName() << "]";
            if (match) std::cout << " ★ Track Match!";
            std::cout << "\n";
            assignStack.push({ci, selected});
            ++ci;
        } else {
            std::cout << "     → 잘못된 번호.\n";
        }
    }
    std::cout << "\n";
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
    static const int W = 58;
    std::cout << "\n";
    UI::line(W, '=');
    std::cout << "  행동 처리\n";
    UI::line(W, '=');

    // 캐릭터 행동
    std::cout << "\n  ─ 캐릭터 ──────────────────────────────────────────\n";
    bool anyAction = false;
    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive() || !party[ci]->hasCard()) continue;
        anyAction = true;
        if (party[ci]->getStatus().isStunned()) {
            std::cout << "\n  " << party[ci]->getName() << "  스턴 — 행동 불가\n";
            party[ci]->clearAssignedCard();
            continue;
        }
        const Card& card = party[ci]->getAssignedCard();
        bool trackMatch  = card.isTrackCard() && party[ci]->hasTrack(card.getTrack());
        std::cout << "\n  " << party[ci]->getName()
                  << "  → [" << card.getName() << "]";
        if (trackMatch) std::cout << "   ★ Track Match!";
        std::cout << "\n";
        currentActorIdx = ci;
        resolveCard(card, *party[ci], true, firstLivingEnemy());
        currentActorIdx = -1;
        party[ci]->clearAssignedCard();
    }
    if (!anyAction) std::cout << "  (행동 없음)\n";

    // 적 행동
    std::cout << "\n  ─ 적 ──────────────────────────────────────────────\n";
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;
        if (enemies[ei].getStatus().isStunned()) {
            std::cout << "\n  " << enemies[ei].getName() << "  스턴 — 행동 불가\n";
            Card dummy; enemies[ei].executeAndQueue(dummy);
            continue;
        }
        Card card;
        if (!enemies[ei].executeAndQueue(card)) continue;
        std::cout << "\n  " << enemies[ei].getName()
                  << "  → [" << card.getName() << "]\n";
        resolveCard(card, enemies[ei], false, -1);
    }
    std::cout << "\n";
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
    UI::header("전투 보상  ─  카드를 선택하세요", W);

    Card offers[BATTLE_REWARD_COUNT];
    int offered = 0;
    for (int attempt = 0; attempt < reg.size() * 2 && offered < BATTLE_REWARD_COUNT; ++attempt) {
        Card c;
        if (reg.getAt(rand() % reg.size(), c)) offers[offered++] = c;
    }

    for (int i = 0; i < offered; ++i) {
        std::cout << "\n";
        UI::line(W, '-');
        std::cout << "  보상 " << (i + 1) << " / " << offered << "\n";
        UI::line(W, '-');
        std::cout << "  ";
        offers[i].print();
        std::cout << "\n\n";
        std::cout << "  [a] 덱에 추가   [r] 기존 카드와 교체   [s] 스킵 > ";
        std::string line;
        std::getline(std::cin, line);
        char ch = line.empty() ? 's' : line[0];

        if (ch == 'a') {
            pool.addCard(offers[i]);
            std::cout << "  → 덱에 추가됨. (총 " << pool.size() << "장)\n";
        } else if (ch == 'r') {
            std::cout << "\n";
            pool.print();
            std::cout << "\n  교체할 카드 번호 > ";
            std::getline(std::cin, line);
            int idx = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? -1 : std::stoi(line);
            if (pool.removeCard(idx)) {
                pool.addCard(offers[i]);
                std::cout << "  → 교체 완료.\n";
            } else {
                std::cout << "  → 잘못된 번호 — 스킵.\n";
            }
        } else {
            std::cout << "  → 스킵.\n";
        }
    }
    std::cout << "\n";
    UI::line(58, '=');
}

bool Battle::run() {
    for (int i = 0; i < enemyCount; ++i) enemies[i].prepareIntent();

    while (!battleOver) {
        ++turnNumber;
        UI::clear();
        displayBattleState();
        drawPhase();
        assignPhase();
        UI::line(56, '-');
        std::cout << "  행동 처리\n";
        UI::line(56, '-');
        executePhase();
        statusTickPhase();
        checkBattleEnd();
        if (!battleOver) {
            endTurnPhase();
            UI::pause();
        }
    }

    if (playerWon) {
        UI::banner("VICTORY!  모든 적을 처치했습니다.");
        UI::pause();
        rewardPhase();
    } else {
        UI::banner("DEFEATED...", "파티가 전멸했습니다.");
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
