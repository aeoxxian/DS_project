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
    {   EnemyDef def("Fission Hound", 60, 8, 4, Track::Nuclear);
        addCardByName(def, cards, "Strike");
        addCardByName(def, cards, "Poison Dart");
        addCardByName(def, cards, "Fission Burst");
        reg.registerMonster(def); }

    {   EnemyDef def("Reactor Wraith", 100, 12, 6, Track::Nuclear);
        addCardByName(def, cards, "Radiation Pulse");
        addCardByName(def, cards, "Containment Breach");
        addCardByName(def, cards, "Guard");
        reg.registerMonster(def); }

    // ── NewMaterial ───────────────────────────────────────────────────────────
    {   EnemyDef def("Nano Swarm", 50, 7, 5, Track::NewMaterial);
        addCardByName(def, cards, "Double Strike");
        addCardByName(def, cards, "Disarm");
        addCardByName(def, cards, "Nano Blade");
        reg.registerMonster(def); }

    {   EnemyDef def("Carbon Golem", 120, 10, 14, Track::NewMaterial);
        addCardByName(def, cards, "Composite Shield");
        addCardByName(def, cards, "Graphene Edge");
        addCardByName(def, cards, "Heavy Blow");
        reg.registerMonster(def); }

    // ── Hydrogen ──────────────────────────────────────────────────────────────
    {   EnemyDef def("Cryo Crawler", 55, 9, 5, Track::Hydrogen);
        addCardByName(def, cards, "Cryo Jet");
        addCardByName(def, cards, "Disarm");
        addCardByName(def, cards, "Strike");
        reg.registerMonster(def); }

    {   EnemyDef def("Plasma Leech", 80, 11, 6, Track::Hydrogen);
        addCardByName(def, cards, "Fuel Cell Blast");
        addCardByName(def, cards, "Plasma Arc");
        addCardByName(def, cards, "Combustion");
        reg.registerMonster(def); }

    // ── EcoTech ───────────────────────────────────────────────────────────────
    {   EnemyDef def("Gust Wraith", 45, 8, 4, Track::EcoTech);
        addCardByName(def, cards, "Gust Slash");
        addCardByName(def, cards, "Smoke Veil");  // Self evade — 적 자신에게 적용
        addCardByName(def, cards, "Wind Shear");
        reg.registerMonster(def); }

    {   EnemyDef def("Turbine Beast", 90, 13, 8, Track::EcoTech);
        addCardByName(def, cards, "Turbine Kick");
        addCardByName(def, cards, "Tailwind");    // Party ATK 버프 — 적 파티에 적용됨 (sourceIsPlayer=false)
        addCardByName(def, cards, "Heavy Blow");
        reg.registerMonster(def); }

    // ── AI ────────────────────────────────────────────────────────────────────
    {   EnemyDef def("Rogue Drone", 55, 9, 5, Track::AI);
        addCardByName(def, cards, "Algorithm Strike");
        addCardByName(def, cards, "Data Wipe");
        addCardByName(def, cards, "Predictive Aim");
        reg.registerMonster(def); }

    {   EnemyDef def("Neural Phantom", 95, 11, 6, Track::AI);
        addCardByName(def, cards, "Neural Surge");
        addCardByName(def, cards, "Recursive Strike");
        addCardByName(def, cards, "Hotfix");      // 자가 HP 회복
        reg.registerMonster(def); }

    // ── Grid ──────────────────────────────────────────────────────────────────
    {   EnemyDef def("Static Fiend", 65, 8, 6, Track::Grid);
        addCardByName(def, cards, "Blackout Strike");
        addCardByName(def, cards, "Static Field");
        addCardByName(def, cards, "Arc Flash");
        reg.registerMonster(def); }

    {   EnemyDef def("Overload Titan", 130, 14, 10, Track::Grid);
        addCardByName(def, cards, "Power Surge");
        addCardByName(def, cards, "Capacitor");   // 자가 ATK 버프 + 방어막
        addCardByName(def, cards, "Overvoltage");
        reg.registerMonster(def); }

    // ── 보스 ─────────────────────────────────────────────────────────────────
    {   EnemyDef def("NEXUS Core", 300, 20, 15, Track::AI, true);
        addCardByName(def, cards, "Targeting Matrix");
        addCardByName(def, cards, "Stack Overflow");
        addCardByName(def, cards, "Encryption");  // 자가 block + DEF 버프
        addCardByName(def, cards, "Singularity");
        reg.registerMonster(def); }

    {   EnemyDef def("Meltdown Titan", 280, 22, 12, Track::Nuclear, true);
        addCardByName(def, cards, "Irradiate");
        addCardByName(def, cards, "Critical Mass");
        addCardByName(def, cards, "Containment Breach");
        addCardByName(def, cards, "Detonate");
        reg.registerMonster(def); }

    {   EnemyDef def("Storm Colossus", 260, 18, 14, Track::EcoTech, true);
        addCardByName(def, cards, "Wind Shear");
        addCardByName(def, cards, "Eye of Storm"); // 자가 evade + heal
        addCardByName(def, cards, "Tornado");
        addCardByName(def, cards, "Gaia's Blessing"); // Party 힐 — 적 파티에 적용됨 (sourceIsPlayer=false)
        reg.registerMonster(def); }
}
