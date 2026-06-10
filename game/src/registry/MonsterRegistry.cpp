#include "registry/MonsterRegistry.h"
#include "registry/CardRegistry.h"
#include <iostream>

MonsterRegistry::MonsterRegistry() : count(0) {}

MonsterRegistry& MonsterRegistry::instance() {
    static MonsterRegistry registry;
    return registry;
}

bool MonsterRegistry::registerMonster(const EnemyDef& def) {
    if (count >= MAX_MONSTER_REGISTRY_SIZE) return false;
    entries[count++] = def;
    return true;
}

bool MonsterRegistry::getAt(int index, EnemyDef& out) const {
    if (index < 0 || index >= count) return false;
    out = entries[index]; return true;
}

bool MonsterRegistry::findByName(const std::string& name, EnemyDef& out) const {
    for (int i = 0; i < count; ++i)
        if (entries[i].name == name) { out = entries[i]; return true; }
    return false;
}

int MonsterRegistry::size() const { return count; }

void MonsterRegistry::printAll() const {
    std::cout << "=== Monster Registry (" << count << ") ===\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  [" << i << "] " << entries[i].name
                  << "  HP:" << entries[i].baseHP << "\n";
}

// ── 헬퍼 ─────────────────────────────────────────────────────────────────────

static void addCardByName(EnemyDef& def, CardRegistry& cards, const std::string& name) {
    Card card;
    if (cards.findByName(name, card))
        def.addCard(card);
    else
        std::cout << "[MonsterRegistry] missing card: " << name << "\n";
}

// ── 몬스터 등록 ───────────────────────────────────────────────────────────────

void registerAllMonsters() {
    MonsterRegistry& reg   = MonsterRegistry::instance();
    CardRegistry&    cards = CardRegistry::instance();

    // ── Nuclear ───────────────────────────────────────────────────────────────
    {   EnemyDef def("Fission Hound", 40, 8, 4, Track::Nuclear);
        addCardByName(def, cards, "타격");
        addCardByName(def, cards, "독침");
        addCardByName(def, cards, "핵분열 폭발");
        reg.registerMonster(def); }

    {   EnemyDef def("Reactor Wraith", 65, 12, 6, Track::Nuclear);
        addCardByName(def, cards, "방사선 파동");
        addCardByName(def, cards, "격납 붕괴");
        addCardByName(def, cards, "수비");
        reg.registerMonster(def); }

    // ── NewMaterial ───────────────────────────────────────────────────────────
    {   EnemyDef def("Nano Swarm", 35, 7, 5, Track::NewMaterial);
        addCardByName(def, cards, "이중 타격");
        addCardByName(def, cards, "무장 해제");
        addCardByName(def, cards, "나노 블레이드");
        reg.registerMonster(def); }

    {   EnemyDef def("Carbon Golem", 80, 10, 14, Track::NewMaterial);
        addCardByName(def, cards, "복합 방패");
        addCardByName(def, cards, "그래핀 칼날");
        addCardByName(def, cards, "강타");
        reg.registerMonster(def); }

    // ── Hydrogen ──────────────────────────────────────────────────────────────
    {   EnemyDef def("Cryo Crawler", 40, 9, 5, Track::Hydrogen);
        addCardByName(def, cards, "극저온 분사");
        addCardByName(def, cards, "무장 해제");
        addCardByName(def, cards, "타격");
        reg.registerMonster(def); }

    {   EnemyDef def("Plasma Leech", 55, 11, 6, Track::Hydrogen);
        addCardByName(def, cards, "연료전지 폭발");
        addCardByName(def, cards, "플라즈마 아크");
        addCardByName(def, cards, "연소");
        reg.registerMonster(def); }

    // ── EcoTech ───────────────────────────────────────────────────────────────
    {   EnemyDef def("Gust Wraith", 30, 8, 4, Track::EcoTech);
        addCardByName(def, cards, "돌풍 베기");
        addCardByName(def, cards, "연막");
        addCardByName(def, cards, "윈드시어");
        reg.registerMonster(def); }

    {   EnemyDef def("Turbine Beast", 60, 13, 8, Track::EcoTech);
        addCardByName(def, cards, "터빈 킥");
        addCardByName(def, cards, "순풍");
        addCardByName(def, cards, "강타");
        reg.registerMonster(def); }

    // ── AI ────────────────────────────────────────────────────────────────────
    {   EnemyDef def("Rogue Drone", 40, 9, 5, Track::AI);
        addCardByName(def, cards, "알고리즘 타격");
        addCardByName(def, cards, "데이터 삭제");
        addCardByName(def, cards, "예측 조준");
        reg.registerMonster(def); }

    {   EnemyDef def("Neural Phantom", 65, 11, 6, Track::AI);
        addCardByName(def, cards, "신경망 과부하");
        addCardByName(def, cards, "재귀 타격");
        addCardByName(def, cards, "긴급 패치");
        reg.registerMonster(def); }

    // ── Grid ──────────────────────────────────────────────────────────────────
    {   EnemyDef def("Static Fiend", 45, 8, 6, Track::Grid);
        addCardByName(def, cards, "정전 타격");
        addCardByName(def, cards, "정전기장");
        addCardByName(def, cards, "아크 방전");
        reg.registerMonster(def); }

    {   EnemyDef def("Overload Titan", 90, 14, 10, Track::Grid);
        addCardByName(def, cards, "전력 과부하");
        addCardByName(def, cards, "축전기");
        addCardByName(def, cards, "과전압");
        reg.registerMonster(def); }

    // ── 보스 ─────────────────────────────────────────────────────────────────
    {   EnemyDef def("NEXUS Core", 200, 20, 15, Track::AI, true);
        addCardByName(def, cards, "조준 행렬");
        addCardByName(def, cards, "스택 오버플로");
        addCardByName(def, cards, "암호화");
        addCardByName(def, cards, "특이점");
        reg.registerMonster(def); }

    {   EnemyDef def("Meltdown Titan", 190, 22, 12, Track::Nuclear, true);
        addCardByName(def, cards, "방사선 조사");
        addCardByName(def, cards, "임계 질량");
        addCardByName(def, cards, "격납 붕괴");
        addCardByName(def, cards, "기폭");
        reg.registerMonster(def); }

    {   EnemyDef def("Storm Colossus", 180, 18, 14, Track::EcoTech, true);
        addCardByName(def, cards, "윈드시어");
        addCardByName(def, cards, "폭풍의 눈");
        addCardByName(def, cards, "토네이도");
        addCardByName(def, cards, "대지의 축복");
        reg.registerMonster(def); }
}
