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

Game::Game() : alive(true), battleLogCount(0),
               unlocks("save/unlocks.dat"), deckSave("save/decks.dat") {
    for (int i = 0; i < MAX_CHARACTERS; ++i) charTotalDamage[i] = 0;
    unlocks.load();
    deckSave.load();

    bool loadMode = showTitleScreen();
    if (loadMode && tryLoadPreset()) return;  // party+deck 모두 복원됨
    selectParty();
    selectStartingDeck();
}

bool Game::showTitleScreen() {
    static const int W = 62;
    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("D U N G E O N   E X P L O R E R", W);
    UI::boxCenter("지식의 상아탑", W);
    UI::boxMid(W);
    if (deckSave.isEmpty()) {
        UI::boxLeft("  [N]  새 게임", W);
        UI::boxDiv(W);
        UI::boxLeft("  저장된 덱 없음  ─  첫 탐험은 기본 덱으로 시작", W);
    } else {
        UI::boxLeft("  [N]  새 게임", W);
        UI::boxLeft("  [L]  덱 불러오기", W);
        UI::boxDiv(W);
        UI::boxLeft("  저장된 덱:", W);
        for (int i = 0; i < deckSave.size(); ++i) {
            const DeckPreset& p = deckSave.getAt(i);
            // 캐릭터 요약
            std::string chars;
            for (int c = 0; c < p.charCount; ++c) {
                if (c > 0) chars += " / ";
                chars += p.charNames[c];
            }
            std::string row = "  [" + std::to_string(i) + "]  " + p.name;
            UI::boxLeft(row, W);
            if (!chars.empty())
                UI::boxLeft("       " + chars + "  (" + std::to_string(p.cardCount) + "장)", W);
        }
    }
    UI::boxBot(W);
    std::cout << "\n";

    while (true) {
        std::cout << "  > ";
        std::string line; std::getline(std::cin, line);
        if (line == "N" || line == "n") return false;
        if (!deckSave.isEmpty() && (line == "L" || line == "l")) return true;
        if (deckSave.isEmpty() && line.empty()) return false;
    }
}

bool Game::tryLoadPreset() {
    static const int W = 62;
    CharacterRoster& roster = CharacterRoster::instance();
    CardRegistry&    creg   = CardRegistry::instance();

    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("덱 불러오기", W);
    UI::boxDiv(W);

    for (int i = 0; i < deckSave.size(); ++i) {
        const DeckPreset& p = deckSave.getAt(i);
        std::string hdr = "  [" + std::to_string(i) + "]  " + p.name
                        + "  (" + std::to_string(p.cardCount) + "장)";
        UI::boxLeft(hdr, W);
        // 캐릭터 줄
        if (p.charCount > 0) {
            std::string chars = "       ";
            for (int c = 0; c < p.charCount; ++c) {
                if (c > 0) chars += "  ·  ";
                CharacterDef def;
                if (roster.findByName(p.charNames[c], def))
                    chars += p.charNames[c] + "[" + trackToString(def.track) + "]";
                else
                    chars += p.charNames[c] + "[?]";
            }
            UI::boxLeft(chars, W);
        }
        if (i < deckSave.size() - 1) UI::boxDiv(W);
    }

    UI::boxDiv(W);
    UI::boxLeft("  번호 선택   [d <번호>] 삭제   [n] 새 게임으로", W);
    UI::boxBot(W);
    std::cout << "\n";

    while (true) {
        std::cout << "  > ";
        std::string line; std::getline(std::cin, line);

        if (line == "n" || line == "N") return false;

        // d <번호> 삭제
        if (line.size() >= 3 && line.substr(0, 2) == "d ") {
            int di = -1;
            try { di = std::stoi(line.substr(2)); } catch (...) {}
            if (di >= 0 && di < deckSave.size()) {
                std::string n = deckSave.getAt(di).name;
                deckSave.removeAt(di);
                deckSave.save();
                std::cout << "  삭제됨: " << n << "\n";
                if (deckSave.isEmpty()) return false;
                for (int i = 0; i < deckSave.size(); ++i)
                    std::cout << "  [" << i << "]  " << deckSave.getAt(i).name
                              << "  (" << deckSave.getAt(i).cardCount << "장)\n";
            } else {
                std::cout << "  잘못된 번호.\n";
            }
            continue;
        }

        // 슬롯 번호
        int si = -1;
        if (!line.empty() && line[0] >= '0' && line[0] <= '9')
            try { si = std::stoi(line); } catch (...) {}
        if (si < 0 || si >= deckSave.size()) { std::cout << "  잘못된 입력.\n"; continue; }

        const DeckPreset& preset = deckSave.getAt(si);

        // ── 캐릭터 복원 ───────────────────────────────────────
        bool charOk = (preset.charCount == MAX_CHARACTERS);
        if (charOk) {
            for (int i = 0; i < MAX_CHARACTERS && charOk; ++i) {
                CharacterDef def;
                if (roster.findByName(preset.charNames[i], def))
                    party[i] = BattleCharacter(def);
                else {
                    std::cout << "  [경고] 캐릭터 없음: " << preset.charNames[i] << "\n";
                    charOk = false;
                }
            }
        }
        if (!charOk) {
            std::cout << "  캐릭터를 직접 선택합니다.\n";
            UI::pause();
            selectParty();
        }

        // ── 카드 복원 ─────────────────────────────────────────
        pool.clear();
        int missing = 0;
        for (int i = 0; i < preset.cardCount; ++i) {
            Card c;
            if (creg.findByName(preset.cardNames[i], c))
                pool.addCard(c);
            else
                ++missing;
        }

        // ── 로드 완료 화면 ────────────────────────────────────
        UI::clear();
        std::cout << "\n";
        UI::boxTop(W);
        UI::boxCenter("로드 완료  ─  " + preset.name, W);
        UI::boxDiv(W);
        UI::boxLeft("  PARTY", W);
        for (int i = 0; i < MAX_CHARACTERS; ++i) {
            std::string row = (i == 0 ? "  ★  " : "     ")
                            + party[i].getName()
                            + "  [" + trackToString(party[i].getTrack()) + "]";
            UI::boxLeft(row, W);
        }
        UI::boxDiv(W);
        UI::boxLeft("  DECK  (" + std::to_string(pool.size()) + "장"
                    + (missing > 0 ? "  경고: " + std::to_string(missing) + "장 누락" : "") + ")", W);
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            std::string row = "  " + std::to_string(i + 1) + ".  " + c.getName();
            UI::boxLeft(row, W);
        }
        UI::boxBot(W);
        std::cout << "\n";
        UI::pause();
        return true;
    }
}

void Game::selectParty() {
    static const int W = 58;
    CharacterRoster& roster = CharacterRoster::instance();

    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("D U N G E O N   E X P L O R E R", W);
    UI::boxMid(W);
    UI::boxCenter("파티 구성  ──  캐릭터 3명 선택", W);
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
            std::cout << "  캐릭터 " << (i + 1) << " > ";
            std::string line;
            std::getline(std::cin, line);

            int idx = 0;
            if (!line.empty() && line[0] >= '0' && line[0] <= '9')
                idx = std::stoi(line);

            CharacterDef def;
            if (!roster.getAt(idx, def)) {
                std::cout << "  잘못된 번호.\n";
                continue;
            }

            // Check for duplicate
            bool dup = false;
            for (int k = 0; k < chosenCount; ++k)
                if (chosenIds[k] == def.id) { dup = true; break; }
            if (dup) {
                std::cout << "  이미 선택된 캐릭터입니다. 다른 캐릭터를 선택해주세요.\n";
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
    UI::boxCenter("★  파티 구성 완료  ★", W);
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
// save <이름> 명령어로 현재 덱 상태를 언제든 저장 가능
static void pickPhase(CardPool& pool, const DynamicArray<Card>& list,
                      int need, const std::string& label,
                      DeckSave* deckSave = nullptr,
                      const BattleCharacter* party = nullptr, int partySize = 0) {
    static const int W = 62;
    int picked = 0;

    while (picked < need) {
        UI::clear();
        std::cout << "\n";
        UI::boxTop(W);
        std::string title = label + "  (" + std::to_string(picked)
                          + "/" + std::to_string(need) + ")";
        UI::boxCenter(title, W);
        UI::boxDiv(W);
        UI::boxLeft("  [번호] 선택   [u] 되돌리기   [save <이름>] 덱 저장", W);
        UI::boxBot(W);

        std::cout << "\n";
        printCardList(list);

        std::cout << "\n  ─── 현재 덱 (" << pool.size() << "/25) ─────────────────────\n";
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            std::cout << "  " << std::right << std::setw(2) << (i + 1)
                      << ".  " << c.getName() << "\n";
        }
        std::cout << "\n  > ";
        std::string line;
        std::getline(std::cin, line);

        if (line.empty()) continue;

        // save 명령어: 현재까지 고른 카드 + 파티 저장
        if (line.size() > 5 && line.substr(0, 5) == "save ") {
            if (deckSave) {
                DeckPreset preset(line.substr(5));
                for (int i = 0; i < partySize; ++i)
                    preset.addChar(party[i].getName());
                for (int i = 0; i < pool.size(); ++i) {
                    Card c; pool.getCard(i, c);
                    preset.addCard(c.getName());
                }
                deckSave->addPreset(preset);
                deckSave->save();
                std::cout << "  저장됨: " << preset.name
                          << "  (" << preset.charCount << "명 / "
                          << preset.cardCount << "장)\n";
            }
            continue;
        }

        if (line == "u" || line == "undo") {
            if (picked > 0) {
                pool.removeCard(pool.size() - 1);
                --picked;
                std::cout << "  마지막 선택 취소.\n";
            } else {
                std::cout << "  취소할 선택 없음.\n";
            }
            continue;
        }

        int idx = -1;
        try { if (!line.empty() && line[0] >= '0' && line[0] <= '9') idx = std::stoi(line); }
        catch (...) {}
        if (idx < 0 || idx >= list.size()) { std::cout << "  잘못된 번호.\n"; continue; }
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

// 카드 교체 UI: offered를 덱의 특정 카드와 교체 (트랙 한도 검사 포함)
void Game::offerCardReplace(const Card& offered) {
    bool isNew = !unlocks.isUnlocked(offered.getName());
    std::cout << "\n  제공 카드: [" << offered.getName() << "]";
    if (offered.isTrackCard())
        std::cout << "  [" << trackToString(offered.getTrack()) << "]";
    if (isNew) std::cout << "  ★신규 해금!";
    std::cout << "\n  " << offered.getDescription() << "\n";

    while (true) {
        std::cout << "\n";
        pool.print();
        std::cout << "\n  교체할 카드 번호 (q=건너뜀) > ";
        std::string line; std::getline(std::cin, line);
        if (line.empty() || line == "q") { std::cout << "  건너뜀.\n"; return; }
        int idx = (!line.empty() && line[0] >= '0' && line[0] <= '9') ? std::stoi(line) : -1;
        Card old;
        if (idx < 0 || !pool.getCard(idx, old)) { std::cout << "  잘못된 번호.\n"; continue; }

        int countAfter = pool.countByTrack(offered.getTrack())
                         - (old.getTrack() == offered.getTrack() ? 1 : 0);
        int lim = (offered.getTrack() == Track::None) ? DECK_COMMON_LIMIT : DECK_TRACK_LIMIT;
        if (countAfter >= lim) {
            std::cout << "  [" << trackToString(offered.getTrack())
                      << "] 한도 " << lim << "장 초과 -- 다른 카드를 선택하세요.\n";
            continue;
        }
        pool.removeCard(idx);
        addCardAndUnlock(offered);
        std::cout << "  [" << old.getName() << "]  →  [" << offered.getName() << "]\n";
        return;
    }
}

// 공용 카드 또는 파티 트랙에 맞는 카드를 레지스트리에서 랜덤 선택
// 트랙별 한도 (공용 10장, 트랙 5장)를 초과하는 카드는 후보에서 제외
bool Game::pickRandomValidCard(Card& out) const {
    CardRegistry& reg = CardRegistry::instance();

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

// 현재 pool + party를 이름으로 슬롯에 저장
void Game::saveDeckPreset(const std::string& name) {
    DeckPreset preset(name);
    for (int i = 0; i < MAX_CHARACTERS; ++i)
        preset.addChar(party[i].getName());
    for (int i = 0; i < pool.size(); ++i) {
        Card c; pool.getCard(i, c);
        preset.addCard(c.getName());
    }
    deckSave.addPreset(preset);
    deckSave.save();
    std::cout << "  저장됨: " << name
              << "  (" << preset.charCount << "명 / " << preset.cardCount << "장)\n";
}

void Game::selectStartingDeck() {
    static const int W = 62;
    CardRegistry& reg = CardRegistry::instance();

    // ── 첫 게임: 해금 카드 없음 → 기본 덱 자동 지급 ───────────────────────
    if (unlocks.isEmpty()) {
        buildDefaultStarterDeck();
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
        UI::boxLeft("  처음이라 기본 덱이 자동 지급됩니다.", W);
        UI::boxLeft("  탐험 중 카드를 획득하면 해금됩니다.", W);
        UI::boxLeft("  다음 게임부터 해금 카드로 덱을 구성!", W);
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

    // ── 해금 카드로 새 덱 빌드 ──────────────────────────────────────────────

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

    // 안내 화면
    UI::clear();
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("덱 구성  ─  해금 카드로 25장 선택", W);
    UI::boxMid(W);
    // 파티 요약
    for (int i = 0; i < MAX_CHARACTERS; ++i) {
        std::string row = (i == 0 ? "  ★  " : "     ")
                        + party[i].getName()
                        + "  [" + trackToString(party[i].getTrack()) + "]";
        UI::boxLeft(row, W);
    }
    UI::boxDiv(W);
    UI::boxLeft("  해금 " + std::to_string(unlocks.size()) + "종"
                "   ·   [save <이름>] 아무 때나 저장", W);
    UI::boxBot(W);
    std::cout << "\n";
    UI::pause();

    // 공용 카드 10장
    pickPhase(pool, commonCards, 10, "공용 카드  (10장)", &deckSave, party, MAX_CHARACTERS);

    // 트랙 카드 캐릭터별 5장
    for (int p = 0; p < MAX_CHARACTERS; ++p) {
        if (trackCards[p].size() == 0) {
            std::string label = party[p].getName()
                              + "  ─  해금 트랙 카드 없음, 공용에서  (5장)";
            pickPhase(pool, commonCards, 5, label, &deckSave, party, MAX_CHARACTERS);
        } else {
            std::string label = party[p].getName()
                              + "  [" + trackToString(party[p].getTrack()) + "]"
                              + "  (5장)";
            pickPhase(pool, trackCards[p], 5, label, &deckSave, party, MAX_CHARACTERS);
        }
    }

    // 완료 화면 + 저장 명령어
    while (true) {
        UI::clear();
        std::cout << "\n";
        UI::boxTop(W);
        UI::boxCenter("덱 완성  ─  25장", W);
        UI::boxDiv(W);
        // 파티
        for (int i = 0; i < MAX_CHARACTERS; ++i) {
            std::string row = (i == 0 ? "  ★  " : "     ")
                            + party[i].getName()
                            + "  [" + trackToString(party[i].getTrack()) + "]";
            UI::boxLeft(row, W);
        }
        UI::boxDiv(W);
        // 덱
        for (int i = 0; i < pool.size(); ++i) {
            Card c; pool.getCard(i, c);
            UI::boxLeft("  " + std::to_string(i + 1) + ".  " + c.getName(), W);
        }
        UI::boxDiv(W);
        UI::boxLeft("  [enter] 탐험 시작   [save <이름>] 덱 저장", W);
        UI::boxBot(W);
        std::cout << "\n  > ";
        std::string line; std::getline(std::cin, line);
        if (line.empty()) break;
        if (line.size() > 5 && line.substr(0, 5) == "save ") {
            saveDeckPreset(line.substr(5));
            continue;
        }
        std::cout << "  [enter] 시작  [save <이름>] 저장\n";
    }
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
            std::cout << "  파티 HP +" << out.value << "\n";
            break;
        case OutcomeType::DamageParty:
            for (int i = 0; i < MAX_CHARACTERS; ++i) party[i].takeDamage(out.value);
            std::cout << "  파티 HP -" << out.value << "\n";
            break;
        case OutcomeType::AddCard: {
            Card c;
            if (pickRandomValidCard(c))
                offerCardReplace(c);
            break;
        }
        case OutcomeType::RemoveCard:
        case OutcomeType::RemoveSelectedCard:
            break;
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
        Item("공격력 포션",   "공격력 +8 (3턴)",     8,  PotionType::AtkUp),
        Item("방어막 포션",   "방어막 +20 (2턴)",    20, PotionType::Shield),
        Item("독 포션",       "적 독 8/턴, 3턴",      8,  PotionType::Poison),
        Item("폭발 포션",     "모든 적 -25 HP",      25, PotionType::Explosive),
    };
    static const int SHOP_SIZE = 7;

    auto printShop = [&]() {
        std::cout << "\n  -- 무료 보급 ------------------------------------\n";
        std::cout << "  아이템 하나를 무료로 획득할 수 있습니다.  인벤토리: "
                  << inventory.size() << "/" << MAX_POTIONS << "\n\n";
        std::cout << "  [ 카드 ]\n";
        std::cout << "  [0] 랜덤 카드\n\n";
        std::cout << "  [ 포션 ]\n";
        for (int i = 0; i < SHOP_SIZE; ++i)
            std::cout << "  [" << (i + 1) << "] "
                      << std::left << std::setw(16) << SHOP_POTIONS[i].getName()
                      << "  " << SHOP_POTIONS[i].getDescription() << "\n";
        std::cout << "\n  [q] 건너뜀\n";
        std::cout << "  --------------------------------------------------\n";
    };

    printShop();
    while (true) {
        std::cout << "  > ";
        std::string line; std::getline(std::cin, line);
        if (line.empty() || line == "q") { std::cout << "  건너뜁니다.\n"; break; }
        if (!(line[0] >= '0' && line[0] <= '9')) { printShop(); continue; }
        int choice = std::stoi(line);

        if (choice == 0) {
            Card c;
            if (pickRandomValidCard(c))
                offerCardReplace(c);
            else
                std::cout << "  유효한 카드가 없습니다.\n";
            break;
        } else if (choice >= 1 && choice <= SHOP_SIZE) {
            int idx = choice - 1;
            if (inventory.size() >= MAX_POTIONS) {
                std::cout << "  인벤토리가 가득 찼습니다.\n";
                inventory.print();
                std::cout << "  교체할 아이템 번호 (취소: 엔터) > ";
                std::string swapLine; std::getline(std::cin, swapLine);
                if (!swapLine.empty() && swapLine[0] >= '0' && swapLine[0] <= '9') {
                    Item old;
                    if (inventory.takeAt(std::stoi(swapLine), old)) {
                        inventory.addItem(SHOP_POTIONS[idx]);
                        std::cout << "  [" << old.getName() << "] -> ["
                                  << SHOP_POTIONS[idx].getName() << "]\n";
                    } else {
                        std::cout << "  잘못된 번호 -- 취소됩니다.\n";
                    }
                } else {
                    std::cout << "  취소.\n";
                }
            } else {
                inventory.addItem(SHOP_POTIONS[idx]);
                std::cout << "  획득: [" << SHOP_POTIONS[idx].getName() << "]\n";
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
    std::cout << "\n  [ 파티 상태 ]\n";
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
    std::cout << "  전투 기록 (피해량 순 정렬)\n";
    UI::line(54, '=');
    std::cout << "  전투                  가함   받음   턴\n";
    UI::line(54, '-');
    for (int i = 0; i < battleLogCount; ++i) {
        const BattleStats& s = sorted[i];
        std::string label = s.isBoss ? "[보스] " + s.label : s.label;
        std::cout << "  " << std::left << std::setw(21) << label
                  << std::right
                  << std::setw(6) << s.damageDealt
                  << std::setw(7) << s.damageTaken
                  << std::setw(7) << s.turns << "\n";
    }
    UI::line(54, '=');

    std::cout << "\n  전투 효율 순위 (가함 - 받음)\n";
    UI::line(54, '-');
    scoreTree.printDescending();
    UI::line(54, '=');

    // Character total damage ranking
    {
        ScoreRecord charRecords[MAX_CHARACTERS];
        for (int i = 0; i < MAX_CHARACTERS; ++i)
            charRecords[i] = ScoreRecord(party[i].getName(), charTotalDamage[i]);
        sortScoresDescending(charRecords, MAX_CHARACTERS);

        std::cout << "\n  캐릭터 총 피해량 순위\n";
        UI::line(54, '-');
        for (int i = 0; i < MAX_CHARACTERS; ++i)
            std::cout << "  " << std::right << std::setw(2) << (i + 1) << ".  "
                      << std::left << std::setw(16) << charRecords[i].name
                      << "  합계: " << charRecords[i].score << "\n";
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

        std::cout << "\n  최종 덱 카드 공격력 순위\n";
        UI::line(54, '-');
        for (int i = 0; i < poolSize; ++i)
            std::cout << "  " << std::right << std::setw(2) << (i + 1) << ".  "
                      << std::left << std::setw(20) << cardRecords[i].name
                      << "  공격력: " << cardRecords[i].score << "\n";
        UI::line(54, '=');

        delete[] cardRecords;
    }
}

// ── Main game loop ─────────────────────────────────────────────────────────────

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
                std::cout << "  이전 방으로 돌아갔습니다.\n";
            else
                std::cout << "  취소할 이동이 없습니다.\n";
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
            std::cout << "  알 수 없는 명령어. h=도움말\n";
            UI::pause();
            continue;
        }

        // ── Room effects ─────────────────────────────────────────────────────
        RoomType type = map.currentType();

        if (map.isCleared()) {
            if (type != RoomType::Start && type != RoomType::Stairs)
                std::cout << "\n  이미 클리어한 방입니다.\n";
        } else {
            map.clearCurrent();
            switch (type) {
                case RoomType::Battle:
                    UI::banner("  ⚔  전투!");
                    handleBattle();
                    break;
                case RoomType::Event:
                    UI::banner("  !  이벤트!");
                    handleEvent();
                    break;
                case RoomType::Rest:
                    UI::banner("  휴식 장소를 발견했습니다.");
                    handleRest();
                    break;
                case RoomType::Boss:
                    UI::banner("  !!  보스 전투  !!", "  강력한 적이 기다리고 있습니다!");
                    handleBattle(true);
                    break;
                case RoomType::Start:   break;
                case RoomType::Stairs:  break;
            }
        }

        if (!alive) break;

        // ── Stairs handling ──────────────────────────────────────────────────
        if (map.isAtStairs()) {
            std::cout << "\n  다음 층으로 이동하시겠습니까? (y/n) > ";
            std::string ans;
            std::getline(std::cin, ans);
            if (!ans.empty() && ans[0] == 'y') {
                if (map.advanceFloor()) {
                    std::cout << "  -> 다음 층으로 이동했습니다!\n";
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
        UI::banner("★  던전  클리어  ★",
                   "  보스 격파! 던전 탈출!");
    } else {
        UI::banner("게  임  오  버",
                   "  파티 전원이 쓰러졌습니다...");
    }
    printRunSummary();
}
