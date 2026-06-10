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

Game::Game() : alive(true), gold(0), battleLogCount(0) {
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
        std::cout << "  번호  이름                트랙           HP   ATK  DEF\n";
        std::cout << "  ─────────────────────────────────────────────────────\n";
        for (int i = 0; i < roster.size(); ++i) {
            CharacterDef def;
            roster.getAt(i, def);
            std::cout << "  [" << i << "]   "
                      << std::left  << std::setw(20) << def.name
                      << std::setw(15) << trackToString(def.track)
                      << std::right
                      << std::setw(4) << def.baseHP
                      << std::setw(5) << def.baseAttack
                      << std::setw(5) << def.baseDefend << "\n";
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

void Game::applyOutcome(const EventOutcome& out) {
    switch (out.type) {
        case OutcomeType::HealParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].heal(out.value);
            std::cout << "  파티 전체 HP +" << out.value << " 회복\n";
            break;
        case OutcomeType::DamageParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].takeDamage(out.value);
            std::cout << "  파티 전체 피해 " << out.value << "\n";
            break;
        case OutcomeType::AddCard: {
            CardRegistry& creg = CardRegistry::instance();
            Card c;
            if (creg.size() > 0 && creg.getAt(rand() % creg.size(), c)) {
                pool.addCard(c);
                std::cout << "  카드 획득: [" << c.getName() << "]\n";
            }
            break;
        }
        case OutcomeType::RemoveCard:
            if (!pool.isEmpty()) pool.removeCard(rand() % pool.size());
            break;
        case OutcomeType::RemoveSelectedCard: {
            if (pool.isEmpty()) { std::cout << "  제거할 카드가 없습니다.\n"; break; }
            std::cout << "\n";
            pool.print();
            std::cout << "  제거할 카드 번호 > ";
            std::string line; std::getline(std::cin, line);
            int idx = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? -1 : std::stoi(line);
            if (pool.removeCard(idx))
                std::cout << "  카드 제거 완료.\n";
            else
                std::cout << "  잘못된 번호 — 스킵.\n";
            break;
        }
        case OutcomeType::GainGold:
            gold += out.value;
            std::cout << "  전리품 +" << out.value << " (현재: " << gold << ")\n";
            break;
        case OutcomeType::LoseGold:
            gold = (gold > out.value) ? gold - out.value : 0;
            std::cout << "  전리품 -" << out.value << " (현재: " << gold << ")\n";
            break;
        case OutcomeType::OpenShop:
            handleShop();
            break;
        default: break;
    }
}

void Game::handleShop() {
    std::cout << "\n";
    std::cout << "  ── 수상한 상점 ────────────────────────────────────\n";
    std::cout << "  현재 전리품: " << gold << "\n\n";
    std::cout << "  [0] 무작위 카드 획득       (전리품 2 소모)\n";
    std::cout << "  [1] 파티 전체 HP 20 회복   (전리품 1 소모)\n";
    std::cout << "  [2] 상점 나가기\n";
    std::cout << "  ───────────────────────────────────────────────────\n";

    while (true) {
        std::cout << "  선택 > ";
        std::string line; std::getline(std::cin, line);
        int choice = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? 2 : std::stoi(line);

        if (choice == 0) {
            if (gold < 2) { std::cout << "  전리품이 부족합니다. (필요: 2)\n"; continue; }
            gold -= 2;
            CardRegistry& creg = CardRegistry::instance();
            Card c;
            if (creg.size() > 0 && creg.getAt(rand() % creg.size(), c)) {
                pool.addCard(c);
                std::cout << "  카드 획득: [" << c.getName() << "] (잔여 전리품: " << gold << ")\n";
            }
        } else if (choice == 1) {
            if (gold < 1) { std::cout << "  전리품이 부족합니다. (필요: 1)\n"; continue; }
            gold -= 1;
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].heal(20);
            std::cout << "  파티 전체 HP +20 회복 (잔여 전리품: " << gold << ")\n";
        } else {
            std::cout << "  상점을 나갑니다.\n";
            break;
        }
    }
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

// ── 파티 현황 ─────────────────────────────────────────────────────────────────
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
                  << party[i].getStatus().toString()
                  << "\n";
    }
    std::cout << "\n";
}

void Game::printRunSummary() const {
    if (battleLogCount == 0) return;

    std::cout << "\n  던전 구조 (DungeonGraph DFS)\n";
    UI::line(54, '-');
    map.printGraph();

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

// ── 메인 게임 루프 ─────────────────────────────────────────────────────────────

void Game::run() {
    UI::clear();
    std::cout << "\n  ══════════════════ 던전 입장 ══════════════════\n";
    std::cout << "\n  파티: ";
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
            std::cout << "  w/a/s/d    — 이동 (위/왼쪽/아래/오른쪽)\n";
            std::cout << "  u / undo   — 직전 방으로 되돌리기 (Stack 기반)\n";
            std::cout << "  q          — 현재 위치 유지\n";
            std::cout << "  g / graph  — 던전 그래프 구조 보기 (DFS)\n";
            std::cout << "  l / look   — 맵 + 파티 상태 다시 보기\n";
            std::cout << "  i / inv    — 인벤토리 확인\n";
            std::cout << "  h / help   — 이 도움말\n";
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
                std::cout << "  ← 직전 방으로 되돌아왔습니다.\n";
            else
                std::cout << "  더 이상 되돌릴 수 없습니다.\n";
            UI::pause();
            continue;
        }

        // 이동: w/a/s/d
        if (cmd.size() == 1 && (cmd[0]=='w'||cmd[0]=='a'||cmd[0]=='s'||cmd[0]=='d')) {
            if (!map.move(cmd[0])) {
                UI::pause();
                continue;
            }
        } else {
            std::cout << "  알 수 없는 명령입니다. h=도움말\n";
            UI::pause();
            continue;
        }

        // ── 방 효과 ───────────────────────────────────────────────────────────
        RoomType type = map.currentType();

        if (map.isCleared()) {
            if (type != RoomType::Start && type != RoomType::Stairs)
                std::cout << "\n  이미 클리어한 방입니다.\n";
        } else {
            map.clearCurrent();
            switch (type) {
                case RoomType::Battle:
                    UI::banner("  ⚔  전투 발생!");
                    handleBattle();
                    break;
                case RoomType::Event:
                    UI::banner("  !  이벤트 발생!");
                    handleEvent();
                    break;
                case RoomType::Rest:
                    UI::banner("  휴식처 발견  ─  잠시 숨을 고릅니다.");
                    handleRest();
                    break;
                case RoomType::Boss:
                    UI::banner("  !!  BOSS 전투  !!", "  강력한 적이 기다리고 있습니다!");
                    handleBattle(true);
                    break;
                case RoomType::Start:   break;
                case RoomType::Stairs:  break;
            }
        }

        if (!alive) break;

        // ── 계단방 처리 ───────────────────────────────────────────────────────
        if (map.isAtStairs()) {
            std::cout << "\n  다음 층으로 이동하시겠습니까? (y/n) > ";
            std::string ans;
            std::getline(std::cin, ans);
            if (!ans.empty() && ans[0] == 'y') {
                if (map.advanceFloor()) {
                    std::cout << "  → 다음 층으로 이동했습니다!\n";
                    UI::pause();
                }
            }
            continue;
        }

        // ── 보스 처치 확인 ────────────────────────────────────────────────────
        if (map.isAtBoss() && map.isCleared()) break;
    }

    UI::clear();
    if (alive) {
        UI::banner("★  D U N G E O N  C L E A R !  ★",
                   "  보스를 처치하고 던전을 탈출했습니다!");
    } else {
        UI::banner("G A M E  O V E R",
                   "  파티가 쓰러졌습니다...");
    }
    printRunSummary();
}
