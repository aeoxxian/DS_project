#include "run/Game.h"
#include "core/UI.h"
#include "map/Room.h"
#include "registry/CharacterRoster.h"
#include "registry/CardRegistry.h"
#include "registry/MonsterRegistry.h"
#include "registry/EventRegistry.h"
#include "ds/Sorting.h"
#include "ds/DynamicArray.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

Game::Game() : alive(true), battleLogCount(0), unlocks("save/unlocks.dat") {
    for (int i = 0; i < MAX_CHARACTERS; ++i) charTotalDamage[i] = 0;
    unlocks.load();
    selectParty();
    selectStartingDeck();
}

void Game::selectParty() {
    static const int W = 58;
    CharacterRoster& roster = CharacterRoster::instance();

    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("D U N G E O N   E X P L O R E R", W);
    UI::boxMid(W);
    UI::boxCenter("Party Setup  --  Select 3 characters", W);
    UI::boxDiv(W);

    if (roster.size() == 0) {
        UI::boxLeft("(no characters registered)", W);
    } else {
        std::ostringstream hdr;
        hdr << std::left << std::setw(5) << "No."
            << std::setw(12) << "Name"
            << std::setw(14) << "Track"
            << std::right
            << std::setw(5) << "HP"
            << std::setw(5) << "ATK"
            << std::setw(5) << "DEF";
        UI::boxLeft(hdr.str(), W);
        UI::boxDiv(W);

        for (int i = 0; i < roster.size(); ++i) {
            CharacterDef def;
            roster.getAt(i, def);
            std::ostringstream row;
            row << "[" << i << "]  "
                << std::left  << std::setw(12) << def.name
                << std::setw(14) << trackToString(def.track)
                << std::right
                << std::setw(5) << def.baseHP
                << std::setw(5) << def.baseAttack
                << std::setw(5) << def.baseDefend;
            UI::boxLeft(row.str(), W);
        }
    }

    UI::boxBot(W);
    std::cout << "\n";

    int chosenIds[MAX_CHARACTERS];
    int chosenCount = 0;

    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        while (true) {
            std::cout << "  Character " << (i + 1) << " > ";
            std::string line;
            std::getline(std::cin, line);

            int idx = 0;
            if (!line.empty() && line[0] >= '0' && line[0] <= '9')
                idx = std::stoi(line);

            CharacterDef def;
            if (!roster.getAt(idx, def)) {
                std::cout << "  Invalid number.\n";
                continue;
            }

            // Check for duplicate
            bool dup = false;
            for (int k = 0; k < chosenCount; ++k)
                if (chosenIds[k] == def.id) { dup = true; break; }
            if (dup) {
                std::cout << "  Already selected! Choose a different character.\n";
                continue;
            }

            chosenIds[chosenCount++] = def.id;
            party[i] = BattleCharacter(def);
            UI::typewrite(">>  " + def.name + "  [" + trackToString(def.track) + "]", 12);
            break;
        }
    }

    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("★  Party Ready  ★", W);
    UI::boxDiv(W);
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        std::string row = "  " + std::to_string(i + 1) + ".  " + party[i].getName()
                        + "  [" + trackToString(party[i].getTrack()) + "]";
        UI::boxLeft(row, W);
    }
    UI::boxBot(W);
    std::cout << "\n";
    UI::pause();
}

// 카드 목록 출력 헬퍼
static void printCardList(const DynamicArray<Card>& list) {
    for (int i = 0; i < list.size(); ++i) {
        std::cout << "  [" << std::setw(2) << i << "] "
                  << std::left << std::setw(18) << list[i].getName()
                  << "  " << list[i].getDescription() << "\n";
    }
}

// 단계별 카드 선택: list에서 need장을 골라 pool에 추가
// 헤더: "Common (0/10)" 형태
static void pickPhase(CardPool& pool, const DynamicArray<Card>& list,
                      int need, const std::string& label) {
    static const int W = 58;
    int picked = 0;

    while (picked < need) {
        UI::clear();
        std::cout << "\n";
        UI::boxTop(W);
        std::string title = label + "  (" + std::to_string(picked)
                          + "/" + std::to_string(need) + ")";
        UI::boxCenter(title, W);
        UI::boxDiv(W);
        UI::boxLeft("[u]  Undo last pick", W);
        UI::boxBot(W);

        std::cout << "\n";
        printCardList(list);

        // Mini deck summary
        std::cout << "\n  -- Deck so far (" << pool.size() << "/25) --\n";
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            std::cout << "  " << i << ". " << c.getName() << "\n";
        }

        std::cout << "\n  > ";
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) continue;

        if (line == "u" || line == "undo") {
            if (picked > 0) {
                pool.removeCard(pool.size() - 1);
                --picked;
                std::cout << "  Removed last pick.\n";
            } else {
                std::cout << "  Nothing to undo.\n";
            }
            continue;
        }

        int idx = -1;
        if (line[0] >= '0' && line[0] <= '9') idx = std::stoi(line);
        if (idx < 0 || idx >= list.size()) {
            std::cout << "  Invalid index.\n";
            continue;
        }
        pool.addCard(list[idx]);
        ++picked;
    }
}

// 첫 게임 기본 덱 (공용 기초 카드 25장)
void Game::buildDefaultStarterDeck() {
    static const char* DEFAULTS[] = {
        "타격","타격","타격","타격","타격",
        "강타","강타",
        "정밀 사격","정밀 사격",
        "수비","수비","수비",
        "방어 태세","방어 태세",
        "방벽",
        "치유","치유","치유",
        "숨 고르기","숨 고르기",
        "응급 처치",
        "집중",
        "베어가르기",
        "무모한 휘두르기",
        "전투 함성"
    };
    static const int N = 25;
    CardRegistry& reg = CardRegistry::instance();
    for (int i = 0; i < N; ++i) {
        Card c;
        if (reg.findByName(DEFAULTS[i], c)) pool.addCard(c);
    }
}

// 카드를 덱에 추가하고 해금 목록에 등록 후 저장
void Game::addCardAndUnlock(const Card& c) {
    pool.addCard(c);
    if (unlocks.unlock(c.getName()))
        unlocks.save();
}

// 공용 카드 또는 파티 트랙에 맞는 카드를 레지스트리에서 랜덤 선택
bool Game::pickRandomValidCard(Card& out) const {
    CardRegistry& reg = CardRegistry::instance();

    // 유효 후보 수집
    DynamicArray<Card> pool_candidates;
    for (int i = 0; i < reg.size(); ++i) {
        Card c;
        if (!reg.getAt(i, c)) continue;
        if (c.getId() == 32) continue;  // 적 전용
        if (c.getTrack() == Track::None) {
            pool_candidates.pushBack(c);
            continue;
        }
        for (int p = 0; p < MAX_CHARACTERS; ++p) {
            if (party[p].hasTrack(c.getTrack())) {
                pool_candidates.pushBack(c);
                break;
            }
        }
    }
    if (pool_candidates.size() == 0) return false;
    out = pool_candidates[rand() % pool_candidates.size()];
    return true;
}

void Game::selectStartingDeck() {
    static const int W = 58;
    CardRegistry& reg = CardRegistry::instance();

    // ── 첫 게임: 해금 카드 없음 → 기본 덱 자동 지급 ───────────────────────
    if (unlocks.isEmpty()) {
        buildDefaultStarterDeck();
        // 덱 카드 전부 해금 (중복 제외)
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            unlocks.unlock(c.getName());
        }
        unlocks.save();

        UI::clear();
        std::cout << "\n";
        UI::boxTop(W);
        UI::boxCenter("★  첫 번째 탐험  ★", W);
        UI::boxMid(W);
        UI::boxLeft("처음이라 기본 덱이 자동 지급됩니다.", W);
        UI::boxLeft("탐험 중 카드를 획득하면 해금됩니다.", W);
        UI::boxLeft("다음 게임부터 해금 카드로 덱을 구성!", W);
        UI::boxDiv(W);
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            UI::boxLeft("  " + std::to_string(i + 1) + ".  " + c.getName(), W);
        }
        UI::boxBot(W);
        std::cout << "\n";
        UI::pause();
        return;
    }

    // ── 이후 게임: 해금 목록에서 픽 ────────────────────────────────────────

    // 해금된 공용 카드
    DynamicArray<Card> commonCards;
    for (int i = 0; i < reg.size(); ++i) {
        Card c;
        if (!reg.getAt(i, c)) continue;
        if (c.getId() == 32) continue;
        if (c.getTrack() == Track::None && unlocks.isUnlocked(c.getName()))
            commonCards.pushBack(c);
    }

    // 해금된 트랙 카드 (파티 캐릭터 트랙 기준)
    DynamicArray<Card> trackCards[MAX_CHARACTERS];
    for (int p = 0; p < MAX_CHARACTERS; ++p) {
        Track t = party[p].getTrack();
        for (int i = 0; i < reg.size(); ++i) {
            Card c;
            if (!reg.getAt(i, c)) continue;
            if (c.getTrack() == t && unlocks.isUnlocked(c.getName()))
                trackCards[p].pushBack(c);
        }
    }

    // 해금 현황 안내
    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("DECK BUILD  --  해금 카드로 25장 구성", W);
    UI::boxMid(W);
    std::string info = "해금 카드: " + std::to_string(unlocks.size()) + "종";
    UI::boxLeft(info, W);
    UI::boxBot(W);
    std::cout << "\n";
    UI::pause();

    // 공용 카드 10장 선택
    pickPhase(pool, commonCards, 10, "공용 카드  (pick 10)");

    // 트랙 카드 캐릭터별 5장
    for (int p = 0; p < MAX_CHARACTERS; ++p) {
        if (trackCards[p].size() == 0) {
            // 해금된 트랙 카드 없으면 공용에서 추가 선택
            std::string label = party[p].getName()
                              + " ── 해금 트랙 카드 없음, 공용에서  (pick 5)";
            pickPhase(pool, commonCards, 5, label);
        } else {
            std::string label = party[p].getName()
                              + " [" + trackToString(party[p].getTrack()) + "]"
                              + "  (pick 5)";
            pickPhase(pool, trackCards[p], 5, label);
        }
    }

    // 완료 안내
    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("DECK READY  --  25 cards", W);
    UI::boxDiv(W);
    for (int i = 0; i < pool.size(); ++i) {
        Card c; pool.getCard(i, c);
        std::string row = std::to_string(i + 1) + ".  " + c.getName();
        UI::boxLeft(row, W);
    }
    UI::boxBot(W);
    std::cout << "\n";
    UI::pause();
}

void Game::buildPool() {
    CardRegistry& reg = CardRegistry::instance();
    for (int i = 0; i < reg.size(); ++i) {
        Card c;
        if (!reg.getAt(i, c)) continue;
        if (c.getTrack() == Track::None) { pool.addCard(c); continue; }
        for (int p = 0; p < MAX_CHARACTERS; ++p)
            if (party[p].hasTrack(c.getTrack())) { pool.addCard(c); break; }
    }
}

void Game::handleBattle(bool isBoss) {
    MonsterRegistry& reg = MonsterRegistry::instance();
    if (reg.size() == 0) {
        std::cout << "\n  (No monsters registered)\n";
        UI::pause();
        return;
    }

    Battle battle(party, MAX_CHARACTERS, pool, &inventory, &unlocks);

    if (isBoss) {
        EnemyDef def;
        bool found = false;
        for (int i = 0; i < reg.size(); ++i) {
            EnemyDef d;
            if (reg.getAt(i, d) && d.isBoss) { def = d; found = true; }
        }
        if (!found) reg.getAt(reg.size() - 1, def);
        battle.addEnemy(Enemy(def));
    } else {
        int count = 1 + rand() % 2;
        for (int i = 0; i < count; ++i) {
            EnemyDef def;
            int attempts = 0;
            do { reg.getAt(rand() % reg.size(), def); }
            while (def.isBoss && ++attempts < 10);
            battle.addEnemy(Enemy(def));
        }
    }

    bool won = battle.run();
    if (battleLogCount < MAX_MAP_NODES) {
        BattleStats stats = battle.getStats();
        battleLog[battleLogCount++] = stats;
        std::string label = stats.isBoss ? "[BOSS] " + stats.label : stats.label;
        scoreTree.insert(ScoreRecord(label, stats.damageDealt - stats.damageTaken));
    }

    if (!won) { alive = false; return; }
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        party[i].clearStatus();
        charTotalDamage[i] += battle.getStats().charDamage[i];
    }
}

void Game::applyOutcome(const EventOutcome& out) {
    switch (out.type) {
        case OutcomeType::HealParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].heal(out.value);
            std::cout << "  Party HP +" << out.value << "\n";
            break;
        case OutcomeType::DamageParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].takeDamage(out.value);
            std::cout << "  Party took " << out.value << " damage\n";
            break;
        case OutcomeType::AddCard: {
            Card c;
            if (pickRandomValidCard(c)) {
                bool isNew = !unlocks.isUnlocked(c.getName());
                addCardAndUnlock(c);
                std::cout << "  카드 획득: [" << c.getName() << "]"
                          << (isNew ? "  ★신규 해금!" : "") << "\n";
            }
            break;
        }
        case OutcomeType::RemoveCard:
            if (!pool.isEmpty()) pool.removeCard(rand() % pool.size());
            break;
        case OutcomeType::RemoveSelectedCard: {
            if (pool.isEmpty()) { std::cout << "  No cards to remove.\n"; break; }
            std::cout << "\n";
            pool.print();
            std::cout << "  Remove card number > ";
            std::string line; std::getline(std::cin, line);
            int idx = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? -1 : std::stoi(line);
            if (pool.removeCard(idx))
                std::cout << "  Card removed.\n";
            else
                std::cout << "  Invalid -- skipped.\n";
            break;
        }
        case OutcomeType::OpenShop:
            handleShop();
            break;
        default: break;
    }
}

void Game::handleShop() {
    static const Item SHOP_POTIONS[] = {
        Item("소형 회복 포션", "HP +15",              15, PotionType::Heal),
        Item("중형 회복 포션", "HP +30",              30, PotionType::Heal),
        Item("대형 회복 포션", "HP +50",              50, PotionType::Heal),
        Item("공격력 포션",   "ATK +8 (3 turns)",     8,  PotionType::AtkUp),
        Item("방어막 포션",   "Shield +20 (2 turns)", 20, PotionType::Shield),
        Item("독 포션",       "Poison enemy 8/t, 3t", 8,  PotionType::Poison),
        Item("폭발 포션",     "All enemies -25 HP",   25, PotionType::Explosive),
    };
    static const int SHOP_SIZE = 7;

    auto printShop = [&]() {
        std::cout << "\n  -- FREE SUPPLY ------------------------------------\n";
        std::cout << "  Pick one item for free.  Inventory: "
                  << inventory.size() << "/" << MAX_POTIONS << "\n\n";
        std::cout << "  [ Card ]\n";
        std::cout << "  [0] Random card\n\n";
        std::cout << "  [ Potions ]\n";
        for (int i = 0; i < SHOP_SIZE; ++i)
            std::cout << "  [" << (i + 1) << "] "
                      << std::left << std::setw(16) << SHOP_POTIONS[i].getName()
                      << "  " << SHOP_POTIONS[i].getDescription() << "\n";
        std::cout << "\n  [q] Skip\n";
        std::cout << "  --------------------------------------------------\n";
    };

    printShop();
    while (true) {
        std::cout << "  > ";
        std::string line; std::getline(std::cin, line);
        if (line.empty() || line == "q") { std::cout << "  Skipped.\n"; break; }
        if (!(line[0] >= '0' && line[0] <= '9')) { printShop(); continue; }
        int choice = std::stoi(line);

        if (choice == 0) {
            Card c;
            if (pickRandomValidCard(c)) {
                bool isNew = !unlocks.isUnlocked(c.getName());
                addCardAndUnlock(c);
                std::cout << "  카드 획득: [" << c.getName() << "]"
                          << (isNew ? "  ★신규 해금!" : "") << "\n";
            }
            break;
        } else if (choice >= 1 && choice <= SHOP_SIZE) {
            int idx = choice - 1;
            if (inventory.size() >= MAX_POTIONS) {
                std::cout << "  Inventory full.\n";
                inventory.print();
                std::cout << "  Swap item number (Enter to cancel) > ";
                std::string swapLine; std::getline(std::cin, swapLine);
                if (!swapLine.empty() && swapLine[0] >= '0' && swapLine[0] <= '9') {
                    Item old;
                    if (inventory.takeAt(std::stoi(swapLine), old)) {
                        inventory.addItem(SHOP_POTIONS[idx]);
                        std::cout << "  [" << old.getName() << "] -> ["
                                  << SHOP_POTIONS[idx].getName() << "]\n";
                    } else {
                        std::cout << "  Invalid -- cancelled.\n";
                    }
                } else {
                    std::cout << "  Cancelled.\n";
                }
            } else {
                inventory.addItem(SHOP_POTIONS[idx]);
                std::cout << "  Got [" << SHOP_POTIONS[idx].getName() << "]\n";
            }
            break;
        } else {
            printShop();
        }
    }
}

void Game::handleEvent() {
    EventRegistry& reg = EventRegistry::instance();
    if (reg.size() == 0) {
        std::cout << "\n  (No events registered)\n";
        UI::pause();
        return;
    }

    Event ev;
    reg.getAt(rand() % reg.size(), ev);
    int choiceIdx = ev.run();

    std::cout << "\n";
    const EventChoice& chosen = ev.getChoice(choiceIdx);
    for (int i = 0; i < chosen.getOutcomeCount(); ++i)
        applyOutcome(chosen.getOutcome(i));

    bool anyAlive = false;
    for (int i = 0; i < MAX_CHARACTERS; ++i)
        if (party[i].isAlive()) { anyAlive = true; break; }
    if (!anyAlive) alive = false;
    UI::pause();
}

void Game::handleRest() {
    std::cout << "\n";
    inventory.print();
    RestRoom rest;
    rest.enter(party, MAX_CHARACTERS, pool);
}

// ── Party status display ──────────────────────────────────────────────────────
static void printPartyStatus(BattleCharacter party[], int count) {
    std::cout << "\n  [ PARTY STATUS ]\n";
    for (int i = 0; i < count; ++i) {
        if (!party[i].isAlive()) {
            std::cout << "  [" << i << "] " << party[i].getName() << "  -- DEAD\n";
            continue;
        }
        std::cout << "  [" << i << "] "
                  << party[i].getName()
                  << "  [" << trackToString(party[i].getTrack()) << "]"
                  << "  HP " << party[i].getHP() << "/" << party[i].getMaxHP()
                  << " " << UI::hpBar(party[i].getHP(), party[i].getMaxHP())
                  << "  ATK:" << party[i].getAttackPower()
                  << " DEF:" << party[i].getDefend()
                  << party[i].getStatus().toString()
                  << "\n";
    }
    std::cout << "\n";
}

void Game::printRunSummary() const {
    if (battleLogCount == 0) return;

    std::cout << "\n  Dungeon structure (DungeonGraph DFS)\n";
    UI::line(54, '-');
    map.printGraph();

    BattleStats sorted[MAX_MAP_NODES];
    for (int i = 0; i < battleLogCount; ++i) sorted[i] = battleLog[i];
    sortBattleStatsByDamage(sorted, battleLogCount);

    std::cout << "\n";
    UI::line(54, '=');
    std::cout << "  Battle Log (sorted by damage dealt)\n";
    UI::line(54, '=');
    std::cout << "  Battle                Dealt  Taken  Turns\n";
    UI::line(54, '-');
    for (int i = 0; i < battleLogCount; ++i) {
        const BattleStats& s = sorted[i];
        std::string label = s.isBoss ? "[BOSS] " + s.label : s.label;
        std::cout << "  " << std::left << std::setw(21) << label
                  << std::right
                  << std::setw(6) << s.damageDealt
                  << std::setw(7) << s.damageTaken
                  << std::setw(7) << s.turns << "\n";
    }
    UI::line(54, '=');

    std::cout << "\n  Battle efficiency ranking (dealt - taken)\n";
    UI::line(54, '-');
    scoreTree.printDescending();
    UI::line(54, '=');

    // Character total damage ranking
    {
        ScoreRecord charRecords[MAX_CHARACTERS];
        for (int i = 0; i < MAX_CHARACTERS; ++i)
            charRecords[i] = ScoreRecord(party[i].getName(), charTotalDamage[i]);
        sortScoresDescending(charRecords, MAX_CHARACTERS);

        std::cout << "\n  Character total damage ranking\n";
        UI::line(54, '-');
        for (int i = 0; i < MAX_CHARACTERS; ++i)
            std::cout << "  " << std::right << std::setw(2) << (i + 1) << ".  "
                      << std::left << std::setw(16) << charRecords[i].name
                      << "  Total: " << charRecords[i].score << "\n";
        UI::line(54, '=');
    }

    // Final deck card attack ranking
    int poolSize = pool.size();
    if (poolSize > 0) {
        ScoreRecord* cardRecords = new ScoreRecord[poolSize];
        for (int i = 0; i < poolSize; ++i) {
            Card c;
            pool.getCard(i, c);
            int dmg = 0;
            for (int e = 0; e < c.getEffectCount(); ++e) {
                const Effect& ef = c.getEffect(e);
                if (ef.type == EffectType::Attack || ef.type == EffectType::HandScaleAttack)
                    dmg += ef.value;
            }
            for (int e = 0; e < c.getBonusEffectCount(); ++e) {
                const Effect& ef = c.getBonusEffect(e);
                if (ef.type == EffectType::Attack || ef.type == EffectType::HandScaleAttack)
                    dmg += ef.value;
            }
            cardRecords[i] = ScoreRecord(c.getName(), dmg);
        }
        sortScoresDescending(cardRecords, poolSize);

        std::cout << "\n  Final deck card attack ranking\n";
        UI::line(54, '-');
        for (int i = 0; i < poolSize; ++i)
            std::cout << "  " << std::right << std::setw(2) << (i + 1) << ".  "
                      << std::left << std::setw(20) << cardRecords[i].name
                      << "  ATK: " << cardRecords[i].score << "\n";
        UI::line(54, '=');

        delete[] cardRecords;
    }
}

// ── Main game loop ─────────────────────────────────────────────────────────────

void Game::run() {
    UI::clear();
    std::cout << "\n  ══════════════════ DUNGEON ENTER ══════════════════\n";
    std::cout << "\n  Party: ";
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        if (i > 0) std::cout << "  /  ";
        std::cout << party[i].getName();
    }
    std::cout << "\n\n";
    printPartyStatus(party, MAX_CHARACTERS);
    map.printMap();
    UI::pause();

    while (alive) {
        UI::clear();
        printPartyStatus(party, MAX_CHARACTERS);
        map.printMap();

        std::cout << "  > ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "h" || cmd == "help") {
            UI::printHelp();
            UI::pause();
            continue;
        }
        if (cmd == "l" || cmd == "look") continue;
        if (cmd == "q") continue;
        if (cmd == "g" || cmd == "graph") {
            std::cout << "\n";
            map.printGraph();
            UI::pause();
            continue;
        }
        if (cmd == "i" || cmd == "inv") {
            std::cout << "\n";
            inventory.print();
            UI::pause();
            continue;
        }
        if (cmd == "u" || cmd == "undo") {
            if (map.undoMove())
                std::cout << "  Returned to previous room.\n";
            else
                std::cout << "  Nothing to undo.\n";
            UI::pause();
            continue;
        }

        // Move: w/a/s/d
        if (cmd.size() == 1 && (cmd[0]=='w'||cmd[0]=='a'||cmd[0]=='s'||cmd[0]=='d')) {
            if (!map.move(cmd[0])) {
                UI::pause();
                continue;
            }
        } else {
            std::cout << "  Unknown command. h=help\n";
            UI::pause();
            continue;
        }

        // ── Room effects ─────────────────────────────────────────────────────
        RoomType type = map.currentType();

        if (map.isCleared()) {
            if (type != RoomType::Start && type != RoomType::Stairs)
                std::cout << "\n  Room already cleared.\n";
        } else {
            map.clearCurrent();
            switch (type) {
                case RoomType::Battle:
                    UI::banner("  ⚔  BATTLE!");
                    handleBattle();
                    break;
                case RoomType::Event:
                    UI::banner("  !  EVENT!");
                    handleEvent();
                    break;
                case RoomType::Rest:
                    UI::banner("  Rest area found.");
                    handleRest();
                    break;
                case RoomType::Boss:
                    UI::banner("  !!  BOSS BATTLE  !!", "  A powerful enemy awaits!");
                    handleBattle(true);
                    break;
                case RoomType::Start:   break;
                case RoomType::Stairs:  break;
            }
        }

        if (!alive) break;

        // ── Stairs handling ──────────────────────────────────────────────────
        if (map.isAtStairs()) {
            std::cout << "\n  Advance to next floor? (y/n) > ";
            std::string ans;
            std::getline(std::cin, ans);
            if (!ans.empty() && ans[0] == 'y') {
                if (map.advanceFloor()) {
                    std::cout << "  -> Advanced to next floor!\n";
                    UI::pause();
                }
            }
            continue;
        }

        // ── Boss cleared check ───────────────────────────────────────────────
        if (map.isAtBoss() && map.isCleared()) break;
    }

    UI::clear();
    if (alive) {
        UI::banner("★  D U N G E O N  C L E A R !  ★",
                   "  Boss defeated! Dungeon escaped!");
    } else {
        UI::banner("G A M E  O V E R",
                   "  Party has fallen...");
    }
    printRunSummary();
}
