#include "Run.h"
#include "ds/CharacterRoster.h"
#include "ds/CardRegistry.h"
#include <iostream>
#include <cstdlib>

Run::Run() : alive(true) {
    selectCharacters();
    buildStartingPool();
}

// ── 초기 설정 ─────────────────────────────────────────────────────────────────
void Run::selectCharacters() {
    CharacterRoster& roster = CharacterRoster::instance();
    std::cout << "\n=== 캐릭터 선택 (3명) ===\n";
    roster.printAll();

    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        std::cout << "\n  " << (i+1) << "번째 캐릭터 인덱스 > ";
        std::string line; std::getline(std::cin, line);
        int idx = line.empty() ? i : std::stoi(line);

        CharacterDef def;
        if (!roster.getAt(idx, def)) roster.getAt(i, def);
        party[i] = BattleCharacter(def);
        std::cout << "  -> " << def.name << " [" << trackToString(def.track) << "]\n";
    }
}

void Run::buildStartingPool() {
    // 시작 카드풀: 일반 카드 전부 + 각 캐릭터 트랙 카드 1장씩
    CardRegistry& reg = CardRegistry::instance();
    for (int i = 0; i < reg.size(); ++i) {
        Card c;
        if (!reg.getAt(i, c)) continue;

        if (c.getType() == CardType::Normal) {
            pool.addCard(c);
            continue;
        }
        // 트랙 카드: 파티 중 해당 트랙 보유자가 있으면 추가
        for (int p = 0; p < MAX_CHARACTERS; ++p) {
            if (party[p].hasTrack(c.getTrack())) {
                pool.addCard(c);
                break;
            }
        }
    }
}

// ── 상태 출력 ─────────────────────────────────────────────────────────────────
void Run::printPartyStatus() const {
    std::cout << "\n[ 파티 상태 ]\n";
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        std::cout << "  "; party[i].printStatus();
    }
}

// ── 방 처리 ───────────────────────────────────────────────────────────────────
void Run::handleBattle(bool isBoss) {
    MonsterRegistry& reg = MonsterRegistry::instance();
    if (reg.size() == 0) { std::cout << "(몬스터 없음)\n"; return; }

    // party[] 원본 직접 전달 — 전투 중 사망이 파티에 즉시 반영
    Battle battle(party, MAX_CHARACTERS, pool);

    if (isBoss) {
        // 보스: 레지스트리 마지막 몬스터 사용 (TODO: 보스 전용 태그)
        Enemy boss;
        reg.getAt(reg.size() - 1, boss);
        battle.addEnemy(boss);
    } else {
        // 일반 전투: 랜덤 1~2마리
        int count = 1 + rand() % 2;
        for (int i = 0; i < count; ++i) {
            Enemy e;
            reg.getAt(rand() % reg.size(), e);
            battle.addEnemy(e);
        }
    }

    bool won = battle.run();
    if (!won) alive = false;

    // 전투 후 파티 HP 동기화 (Battle이 복사본 사용하므로 추후 개선 필요)
    // TODO: Battle이 party 원본을 직접 참조하도록 리팩토링
}

void Run::handleEvent() {
    EventRegistry& reg = EventRegistry::instance();
    if (reg.size() == 0) { std::cout << "(이벤트 없음 — 추후 추가)\n"; return; }

    Event ev;
    reg.getAt(rand() % reg.size(), ev);
    EventOutcome outcome = ev.run();

    // 결과 적용
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
            if (creg.getAt(rand() % creg.size(), c)) pool.addCard(c);
            break;
        }
        case OutcomeType::RemoveCard:
            if (!pool.isEmpty()) pool.removeCard(rand() % pool.size());
            break;
        case OutcomeType::GainGold:
        case OutcomeType::LoseGold:
            // TODO: 골드 시스템 추가 시 처리
            break;
        case OutcomeType::Nothing:
        default:
            break;
    }

    // 파티 전멸 체크
    bool anyAlive = false;
    for (int i = 0; i < MAX_CHARACTERS; ++i)
        if (party[i].isAlive()) { anyAlive = true; break; }
    if (!anyAlive) alive = false;
}

void Run::handleRest() {
    RestRoom rest;
    rest.enter(party, MAX_CHARACTERS, pool);
}

// ── 메인 루프 ─────────────────────────────────────────────────────────────────
bool Run::start() {
    std::cout << "\n========== 런 시작 ==========\n";
    printPartyStatus();

    // 시작 방 처리 (depth 0, Battle 고정)
    handleBattle();

    while (alive) {
        printPartyStatus();
        map.printMap();

        RoomType next = map.advance();

        switch (next) {
            case RoomType::Battle: handleBattle();      break;
            case RoomType::Event:  handleEvent();       break;
            case RoomType::Rest:   handleRest();        break;
            case RoomType::Boss:   handleBattle(true);  break;
        }

        if (map.isAtBoss()) break; // 보스 처리 후 종료
    }

    if (alive && map.isAtBoss()) {
        std::cout << "\n========== 클리어! ==========\n";
        return true;
    }
    std::cout << "\n========== 런 종료 ==========\n";
    return false;
}
