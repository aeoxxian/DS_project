#include "battle/Battle.h"
#include "run/Item.h"
#include "registry/CardRegistry.h"
#include "core/UI.h"
#include "ds/DynamicArray.h"
#include <iostream>
#include <string>
#include <iomanip>
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
                                  << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                    }
                } else if (targetIdx >= 0 && enemies[targetIdx].isAlive()) {
                    enemies[targetIdx].getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    " << enemies[targetIdx].getName()
                              << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                }
            } else {
                for (int ci = 0; ci < partySize; ++ci) {
                    if (!party[ci]->isAlive()) continue;
                    party[ci]->getStatus().apply(e.stat, e.value, e.duration);
                    std::cout << "    " << party[ci]->getName()
                              << " DEBUFF[" << e.stat << " x" << e.duration << "t]\n";
                }
            }
            break;
        }
        case EffectType::Draw:
            pendingDrawBonus += e.value;
            std::cout << "    Next turn +" << e.value << " draw\n";
            break;
        case EffectType::Swap: {
            if (sourceIsPlayer) {
                std::cout << "    Swap positions (e.g. 0 1) > ";
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
                              << " <-> " << party[b]->getName() << " swapped\n";
                } else {
                    std::cout << "    Invalid input, skipped\n";
                }
            } else {
                if (partySize >= 2) {
                    int a = rand() % partySize;
                    int b; do { b = rand() % partySize; } while (b == a);
                    std::swap(party[a], party[b]);
                    std::cout << "    Forced swap! "
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
    std::cout << "  Drew: " << drawn << " card(s)  (Hand: " << hand.size() << ")\n";
}

void Battle::displayBattleState() const {
    static const int W = 58;
    std::cout << "\n";
    UI::line(W, '=');
    std::cout << "  TURN " << turnNumber << "\n";
    UI::line(W, '=');

    // ── PARTY ───────────────────────────────────────────────────────────────
    UI::section("PARTY", W);
    int front = frontPartyIndex();
    for (int i = 0; i < partySize; ++i) {
        if (!party[i]->isAlive()) {
            std::cout << "  [" << i << "] ------  "
                      << std::left << std::setw(8) << party[i]->getName()
                      << "  DEAD\n";
            continue;
        }
        const Combatant& c = *party[i];
        bool isFront = (i == front);
        std::cout << "  " << (isFront ? "★FRONT" : "  REAR")
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

    // ── ENEMIES ─────────────────────────────────────────────────────────────
    UI::section("ENEMIES", W);
    for (int i = 0; i < enemyCount; ++i) {
        if (!enemies[i].isAlive()) continue;
        const Enemy& e = enemies[i];
        Card intent;
        bool hasIntent = e.peekIntent(intent);
        std::cout << "  [" << i << "] "
                  << std::left << std::setw(16) << e.getName()
                  << " HP " << std::right << std::setw(3) << e.getHP()
                  << "/" << std::setw(3) << e.getMaxHP()
                  << " " << UI::hpBar(e.getHP(), e.getMaxHP())
                  << "  ATK:" << std::setw(2) << e.getAttackPower()
                  << " DEF:" << std::setw(2) << e.getDefend();
        std::string st = e.getStatus().toString();
        if (!st.empty()) std::cout << " " << st;
        std::cout << "\n";
        if (hasIntent) {
            std::cout << "       Next: [" << intent.getName() << "]";
            std::string desc = intent.getDescription();
            if (!desc.empty()) std::cout << "  " << desc;
            std::cout << "\n";
        } else {
            std::cout << "       Next: ???\n";
        }
    }

    std::cout << "\n";
    UI::line(W, '=');
}

void Battle::printHand(int forChar) const {
    static const int W = 58;
    UI::section("HAND  (" + std::to_string(hand.size()) + " cards)", W);

    for (int i = 0; i < hand.slotSize(); ++i) {
        Card c;
        if (!hand.peekCard(i, c)) {
            std::cout << "  [" << i << "] --\n";
            continue;
        }

        bool trackMatch = c.isTrackCard()
                          && forChar >= 0
                          && forChar < partySize
                          && party[forChar]->isAlive()
                          && party[forChar]->hasTrack(c.getTrack());

        std::cout << "  [" << i << "] "
                  << std::left << std::setw(18) << c.getName();

        if (c.isTrackCard())
            std::cout << " [" << std::setw(11) << trackToString(c.getTrack())
                      << (trackMatch ? "★]" : " ]");
        else
            std::cout << "              ";

        std::string desc = c.getDescription();
        if (!desc.empty()) std::cout << "  " << desc;
        std::cout << "\n";
    }
}

void Battle::assignPhase() {
    static const int W = 58;
    assignStack.clear();

    // 첫 번째 살아있는 캐릭터 기준으로 초기 손패 출력
    int firstAlive = 0;
    while (firstAlive < partySize && !party[firstAlive]->isAlive()) ++firstAlive;
    printHand(firstAlive < partySize ? firstAlive : -1);

    std::cout << "\n";
    UI::line(W, '-');
    std::cout << "  ASSIGN CARDS   Alive: " << livingPartyCount();
    std::cout << "   h=help  u=undo  use=potion  switch=swap\n";
    UI::line(W, '-');
    std::cout << "\n";

    int ci = 0;
    while (ci < partySize) {
        if (!party[ci]->isAlive()) { ++ci; continue; }
        if (hand.isEmpty()) { ++ci; continue; }

        bool isFront = (ci == frontPartyIndex());
        std::cout << "  " << (isFront ? "★" : "  ")
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
            std::cout << "\n";
            UI::line(W, '-');
            std::cout << "  ASSIGN CARDS   Alive: " << livingPartyCount();
            std::cout << "   h=help  u=undo  use=potion  switch=swap\n";
            UI::line(W, '-');
            continue;
        }
        if (line == "u" || line == "undo") {
            AssignRecord rec;
            if (assignStack.pop(rec)) {
                hand.restoreCard(rec.slotIdx, rec.card);
                party[rec.ci]->clearAssignedCard();
                std::cout << "  Undone: [" << rec.card.getName() << "] ("
                          << party[rec.ci]->getName() << ")\n";
                printHand(rec.ci);
                std::cout << "\n";
                UI::line(W, '-');
                ci = rec.ci;
            } else {
                std::cout << "  Nothing to undo.\n";
            }
            continue;
        }

        if (line == "use") {
            if (!inv || inv->isEmpty()) {
                std::cout << "  No items in inventory.\n";
                continue;
            }
            inv->print();
            std::cout << "  Item number > ";
            std::getline(std::cin, line);
            int itemIdx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
            Item it;
            if (!inv->takeAt(itemIdx, it)) {
                std::cout << "  Invalid number.\n";
                continue;
            }
            switch (it.getType()) {
                case PotionType::Heal: {
                    std::cout << "  Target character number > ";
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
                    std::cout << "  Target character number > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->getStatus().apply("atk_up", it.getValue(), 3);
                        std::cout << "  " << party[ti]->getName() << " ATK +" << it.getValue() << " (3 turns)\n";
                    }
                    break;
                }
                case PotionType::Shield: {
                    std::cout << "  Target character number > ";
                    std::getline(std::cin, line);
                    int ti = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : frontPartyIndex();
                    if (ti < 0 || ti >= partySize || !party[ti]->isAlive()) ti = frontPartyIndex();
                    if (ti >= 0) {
                        party[ti]->getStatus().apply("shield", it.getValue(), 2);
                        std::cout << "  " << party[ti]->getName() << " Shield +" << it.getValue() << " (2 turns)\n";
                    }
                    break;
                }
                case PotionType::Poison: {
                    std::cout << "  Target enemy number > ";
                    std::getline(std::cin, line);
                    int ei = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : firstLivingEnemy();
                    if (ei < 0 || ei >= enemyCount || !enemies[ei].isAlive()) ei = firstLivingEnemy();
                    if (ei >= 0) {
                        enemies[ei].getStatus().apply("poison", it.getValue(), 3);
                        std::cout << "  " << enemies[ei].getName() << " poisoned (" << it.getValue() << "/turn, 3 turns)\n";
                    }
                    break;
                }
                case PotionType::Explosive: {
                    std::cout << "  Explosion! All enemies take " << it.getValue() << " damage\n";
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
            std::cout << "  Swap two positions (e.g. 0 1) > ";
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
                          << " swapped (uses turn)\n";
                assignStack.clear();
                ++ci;
            } else {
                std::cout << "  Invalid.\n";
            }
            continue;
        }

        int idx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
        Card selected;
        if (hand.removeCard(idx, selected)) {
            party[ci]->assignCard(selected);
            bool match = selected.isTrackCard() && party[ci]->hasTrack(selected.getTrack());
            std::cout << "     -> [" << selected.getName() << "]";
            if (match) std::cout << "  ★ Track Match!";
            std::cout << "\n";
            assignStack.push({ci, idx, selected});
            ++ci;
        } else {
            std::cout << "     -> Invalid card.\n";
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
    std::cout << "  ACTION PHASE\n";
    UI::line(W, '=');

    // Characters act
    std::cout << "\n  -- CHARACTERS --\n";
    bool anyAction = false;
    for (int ci = 0; ci < partySize; ++ci) {
        if (!party[ci]->isAlive() || !party[ci]->hasCard()) continue;
        anyAction = true;
        if (party[ci]->getStatus().isStunned()) {
            std::cout << "\n  " << party[ci]->getName() << "  Stunned -- cannot act\n";
            party[ci]->clearAssignedCard();
            continue;
        }
        const Card& card = party[ci]->getAssignedCard();
        bool trackMatch  = card.isTrackCard() && party[ci]->hasTrack(card.getTrack());
        std::cout << "\n  " << party[ci]->getName()
                  << "  -> [" << card.getName() << "]";
        if (trackMatch) std::cout << "   ★ Track Match!";
        std::cout << "\n";
        currentActorIdx = ci;
        resolveCard(card, *party[ci], true, firstLivingEnemy());
        currentActorIdx = -1;
        party[ci]->clearAssignedCard();
    }
    if (!anyAction) std::cout << "  (no actions)\n";

    // Enemies act
    std::cout << "\n  -- ENEMIES --\n";
    for (int ei = 0; ei < enemyCount; ++ei) {
        if (!enemies[ei].isAlive()) continue;
        if (enemies[ei].getStatus().isStunned()) {
            std::cout << "\n  " << enemies[ei].getName() << "  Stunned -- cannot act\n";
            Card dummy; enemies[ei].executeAndQueue(dummy);
            continue;
        }
        Card card;
        if (!enemies[ei].executeAndQueue(card)) continue;
        std::cout << "\n  " << enemies[ei].getName()
                  << "  -> [" << card.getName() << "]\n";
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
    UI::boxTop(W);
    UI::boxCenter("BATTLE REWARD", W);
    UI::boxCenter("카드 1장을 덱에 추가할 수 있습니다", W);
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

        std::string header = "REWARD  " + std::to_string(i + 1) + " / " + std::to_string(offered);
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

        UI::boxLeft("[a] Add to deck", W);
        UI::boxLeft("[r] Replace a card in deck", W);
        UI::boxLeft("[s] Skip", W);
        UI::boxBot(W);

        std::cout << "  > ";
        std::string line;
        std::getline(std::cin, line);
        char ch = line.empty() ? 's' : line[0];

        if (ch == 'a') {
            pool.addCard(offers[i]);
            if (unlocks) { unlocks->unlock(offers[i].getName()); unlocks->save(); }
            UI::typewrite("Added.  (Deck: " + std::to_string(pool.size()) + " cards)", 12);
        } else if (ch == 'r') {
            std::cout << "\n";
            pool.print();
            std::cout << "\n  Replace card number > ";
            std::getline(std::cin, line);
            int idx = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? -1 : std::stoi(line);
            if (pool.removeCard(idx)) {
                pool.addCard(offers[i]);
                if (unlocks) { unlocks->unlock(offers[i].getName()); unlocks->save(); }
                UI::typewrite("Replaced.", 12);
            } else {
                std::cout << "  Invalid number -- skipped.\n";
            }
        } else {
            std::cout << "  Skipped.\n";
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
        UI::line(56, '-');
        std::cout << "  Resolving...\n";
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
        UI::sleep(300);
        UI::banner("★  V I C T O R Y  ★", "All enemies defeated!");
        UI::pause();
        rewardPhase();
    } else {
        UI::sleep(400);
        UI::banner("G A M E  O V E R", "Party has been wiped out...");
        UI::typewrite("쓰러진 동료들의 이름이 머릿속을 스쳐 지나간다.", 20);
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
