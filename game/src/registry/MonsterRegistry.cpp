#include "registry/MonsterRegistry.h"
#include "effect/Effect.h"
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

// ── 몬스터 등록 ───────────────────────────────────────────────────────────────
//
// 양식:
//   EnemyDef def("이름", HP, ATK, DEF, MGC, Track);
//
//   Card skill1(0, "공격", "단일 타격", Track::None, TargetScope::Single);
//   skill1.addEffect(physAttack(10));
//   def.addSkill(skill1);
//
//   Card skill2(0, "독 분사", "전체 독", Track::None, TargetScope::All);
//   skill2.addEffect(poison(3));
//   def.addSkill(skill2);
//
//   reg.registerMonster(def);
//
// 패턴은 addSkill 순서대로 순환: skill1 → skill2 → skill1 → ...
// ─────────────────────────────────────────────────────────────────────────────

void registerAllMonsters() {
    using namespace Effects;
    MonsterRegistry& reg = MonsterRegistry::instance();

    // ── Nuclear ───────────────────────────────────────────────────────────────
    {   EnemyDef def("Fission Hound", 60, 8, 4, Track::Nuclear);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Reactor Wraith", 100, 12, 6, Track::Nuclear);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── NewMaterial ───────────────────────────────────────────────────────────
    {   EnemyDef def("Nano Swarm", 50, 7, 5, Track::NewMaterial);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Carbon Golem", 120, 10, 14, Track::NewMaterial);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── Hydrogen ──────────────────────────────────────────────────────────────
    {   EnemyDef def("Cryo Crawler", 55, 9, 5, Track::Hydrogen);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Plasma Leech", 80, 11, 6, Track::Hydrogen);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── EcoTech ───────────────────────────────────────────────────────────────
    {   EnemyDef def("Gust Wraith", 45, 8, 4, Track::EcoTech);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Turbine Beast", 90, 13, 8, Track::EcoTech);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── AI ────────────────────────────────────────────────────────────────────
    {   EnemyDef def("Rogue Drone", 55, 9, 5, Track::AI);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Neural Phantom", 95, 11, 6, Track::AI);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── Grid ──────────────────────────────────────────────────────────────────
    {   EnemyDef def("Static Fiend", 65, 8, 6, Track::Grid);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Overload Titan", 130, 14, 10, Track::Grid);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    // ── 보스 (isBoss = true) ──────────────────────────────────────────────────
    {   EnemyDef def("NEXUS Core",     300, 20, 15, Track::AI,      true);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Meltdown Titan", 280, 22, 12, Track::Nuclear, true);
        // TODO: 스킬 추가
        reg.registerMonster(def); }

    {   EnemyDef def("Storm Colossus", 260, 18, 14, Track::EcoTech, true);
        // TODO: 스킬 추가
        reg.registerMonster(def); }
}
