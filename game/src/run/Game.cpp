#include "run/Game.h"
#include "core/UI.h"
#include "map/Room.h"
#include "registry/CharacterRoster.h"
#include "registry/CardRegistry.h"
#include "registry/MonsterRegistry.h"
#include "registry/EventRegistry.h"
#include "ds/Sorting.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

Game::Game() : alive(true), battleLogCount(0) {
    selectParty();
    buildPool();
}

void Game::selectParty() {
    CharacterRoster& roster = CharacterRoster::instance();

    UI::clear();
    std::cout << "\n";
    std::cout << "  ══════════════════ 캐릭터 선택 ══════════════════\n";
    std::cout << "  3명을 차례로 선택하세요.\n\n";

    if (roster.size() == 0) {
        std::cout << "  (등록된 캐릭터 없음 — CharacterRoster.cpp에 추가하세요)\n";
    } else {
        std::cout << "  번호  이름                트랙           HP   ATK  DEF  MGC\n";
        std::cout << "  ─────────────────────────────────────────────────────────────\n";
        for (int i = 0; i < roster.size(); ++i) {
            CharacterDef def;
            roster.getAt(i, def);
            std::cout << "  [" << i << "]   "
                      << std::left  << std::setw(20) << def.name
                      << std::setw(15) << trackToString(def.track)
                      << std::right
                      << std::setw(4) << def.baseHP
                      << std::setw(5) << def.baseAttack
                      << std::setw(5) << def.baseDefend
                      << std::setw(5) << def.baseMagicPower << "\n";
        }
        std::cout << "\n";
    }

    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        std::cout << "  " << (i + 1) << "번째 캐릭터 > ";
        std::string line;
        std::getline(std::cin, line);

        int idx = 0;
        if (!line.empty() && line[0] >= '0' && line[0] <= '9')
            idx = std::stoi(line);

        CharacterDef def;
        if (!roster.getAt(idx, def)) {
            if (roster.size() > 0) roster.getAt(0, def);
        }
        party[i] = BattleCharacter(def);
        std::cout << "  → " << def.name << " [" << trackToString(def.track) << "] 선택\n";
    }

    std::cout << "\n  파티: ";
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        if (i > 0) std::cout << "  /  ";
        std::cout << party[i].getName() << " [" << trackToString(party[i].getTrack()) << "]";
    }
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
        std::cout << "\n  (등록된 몬스터 없음 — MonsterRegistry.cpp에 추가하세요)\n";
        UI::pause();
        return;
    }

    Battle battle(party, MAX_CHARACTERS, pool);

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
    for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].clearStatus();
}

void Game::handleEvent() {
    EventRegistry& reg = EventRegistry::instance();
    if (reg.size() == 0) {
        std::cout << "\n  (등록된 이벤트 없음 — EventRegistry.cpp에 추가하세요)\n";
        UI::pause();
        return;
    }

    Event ev;
    reg.getAt(rand() % reg.size(), ev);
    EventOutcome outcome = ev.run();

    switch (outcome.type) {
        case OutcomeType::HealParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].heal(outcome.value);
            break;
        case OutcomeType::DamageParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].takeDamage(outcome.value);
            break;
        case OutcomeType::AddCard: {
            CardRegistry& creg = CardRegistry::instance();
            Card c;
            if (creg.size() > 0 && creg.getAt(rand() % creg.size(), c)) pool.addCard(c);
            break;
        }
        case OutcomeType::RemoveCard:
            if (!pool.isEmpty()) pool.removeCard(rand() % pool.size());
            break;
        case OutcomeType::GainGold: {
            Item treasure("전리품", "이벤트에서 획득한 아이템", outcome.value);
            inventory.addItem(treasure);
            std::cout << "\n  인벤토리에 추가됨: 전리품 (가치:" << outcome.value << ")\n";
            break;
        }
        default: break;
    }

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

// ── 파티 현황 바 ──────────────────────────────────────────────────────────────
static void printPartyStatus(BattleCharacter party[], int count) {
    std::cout << "\n  [ 파티 현황 ]\n";
    for (int i = 0; i < count; ++i) {
        if (!party[i].isAlive()) {
            std::cout << "  [" << i << "] " << party[i].getName() << "  -- 사망\n";
            continue;
        }
        std::cout << "  [" << i << "] "
                  << party[i].getName()
                  << "  [" << trackToString(party[i].getTrack()) << "]"
                  << "  HP " << party[i].getHP() << "/" << party[i].getMaxHP()
                  << " " << UI::hpBar(party[i].getHP(), party[i].getMaxHP())
                  << "  ATK:" << party[i].getAttackPower()
                  << " DEF:" << party[i].getDefend()
                  << " MGC:" << party[i].getMagicPower()
                  << party[i].getStatus().toString()
                  << "\n";
    }
    std::cout << "\n";
}

void Game::printRunSummary() const {
    if (battleLogCount == 0) return;

    BattleStats sorted[MAX_MAP_NODES];
    for (int i = 0; i < battleLogCount; ++i) sorted[i] = battleLog[i];
    sortBattleStatsByDamage(sorted, battleLogCount);

    std::cout << "\n";
    UI::line(54, '=');
    std::cout << "  런 전투 통계 (입힌 데미지 순)\n";
    UI::line(54, '=');
    std::cout << "  전투                 입힘   받음   턴\n";
    UI::line(54, '-');
    for (int i = 0; i < battleLogCount; ++i) {
        const BattleStats& s = sorted[i];
        std::string label = s.isBoss ? "[BOSS] " + s.label : s.label;
        std::cout << "  " << std::left << std::setw(21) << label
                  << std::right
                  << std::setw(6) << s.damageDealt
                  << std::setw(7) << s.damageTaken
                  << std::setw(5) << s.turns << "\n";
    }
    UI::line(54, '=');

    std::cout << "\n  전투 효율 랭킹 (입힌 데미지 - 받은 데미지)\n";
    UI::line(54, '-');
    scoreTree.printDescending();
    UI::line(54, '=');
}

void Game::run() {
    UI::clear();
    std::cout << "\n  ══════════════════ 던전 입장 ══════════════════\n";
    std::cout << "\n  파티: ";
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        if (i > 0) std::cout << "  /  ";
        std::cout << party[i].getName();
    }
    std::cout << "\n\n";
    map.printMap();
    std::cout << "\n  [첫 번째 방: 전투]\n";
    UI::pause();
    handleBattle();

    while (alive) {
        UI::clear();
        printPartyStatus(party, MAX_CHARACTERS);
        map.printMap();

        // 명령 루프: 진행 전 look/help/inventory/undo 허용
        while (true) {
            std::cout << "\n  [Enter] 진행  [l] 다시보기  [i] 인벤토리  [u] 되돌리기  [h] 도움말 > ";
            std::string cmd;
            std::getline(std::cin, cmd);

            if (cmd == "h" || cmd == "help") {
                std::cout << "  Enter      — 다음 방으로 진행\n";
                std::cout << "  l / look   — 현재 맵과 파티 상태 다시 보기\n";
                std::cout << "  i / inv    — 인벤토리 확인\n";
                std::cout << "  u / undo   — 이전 방으로 되돌리기 (Stack 기반)\n";
                std::cout << "  h / help   — 이 도움말\n";
            } else if (cmd == "l" || cmd == "look") {
                UI::clear();
                printPartyStatus(party, MAX_CHARACTERS);
                map.printMap();
            } else if (cmd == "i" || cmd == "inv") {
                std::cout << "\n";
                inventory.print();
            } else if (cmd == "u" || cmd == "undo") {
                if (map.undoMove()) {
                    std::cout << "  ← 이전 방으로 되돌아왔습니다.\n";
                    UI::clear();
                    printPartyStatus(party, MAX_CHARACTERS);
                    map.printMap();
                } else {
                    std::cout << "  더 이상 되돌릴 수 없습니다.\n";
                }
            } else {
                break;
            }
        }

        RoomType next = map.advance();
        if (!alive) break;

        switch (next) {
            case RoomType::Battle: handleBattle();     break;
            case RoomType::Event:  handleEvent();      break;
            case RoomType::Rest:   handleRest();       break;
            case RoomType::Boss:   handleBattle(true); break;
        }

        if (map.isAtBoss()) break;
    }

    UI::clear();
    if (alive) {
        std::cout << "\n  ╔════════════════════════════════╗\n";
        std::cout << "  ║      ★  C L E A R !  ★         ║\n";
        std::cout << "  ╚════════════════════════════════╝\n";
    } else {
        std::cout << "\n  ╔════════════════════════════════╗\n";
        std::cout << "  ║        GAME  OVER               ║\n";
        std::cout << "  ╚════════════════════════════════╝\n";
    }
    printRunSummary();
}
