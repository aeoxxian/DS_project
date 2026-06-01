#include "ds/MonsterRegistry.h"
#include <iostream>

MonsterRegistry::MonsterRegistry() : count(0) {}

MonsterRegistry& MonsterRegistry::instance() {
    static MonsterRegistry registry;
    return registry;
}

bool MonsterRegistry::registerMonster(const Enemy& enemy) {
    if (count >= MAX_MONSTER_REGISTRY_SIZE) return false;
    entries[count++] = enemy;
    return true;
}

bool MonsterRegistry::getAt(int index, Enemy& out) const {
    if (index < 0 || index >= count) return false;
    out = entries[index];
    return true;
}

bool MonsterRegistry::findByName(const std::string& name, Enemy& out) const {
    for (int i = 0; i < count; ++i) {
        if (entries[i].getName() == name) {
            out = entries[i];
            return true;
        }
    }
    return false;
}

int MonsterRegistry::size() const { return count; }

void MonsterRegistry::printAll() const {
    std::cout << "=== Monster Registry (" << count << ") ===\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  " << i << ". ";
        entries[i].printStatus();
    }
}

// ── Built-in monster definitions ─────────────────────────────────────────────
// To add a monster: append a new registerMonster() call here.
// Enemy(name, maxHp, attackPower, track)

void registerAllMonsters() {
    MonsterRegistry& reg = MonsterRegistry::instance();

    // Nuclear
    reg.registerMonster(Enemy("Fission Hound",    60,  8,  Track::Nuclear));
    reg.registerMonster(Enemy("Reactor Wraith",   100, 12, Track::Nuclear));

    // NewMaterial
    reg.registerMonster(Enemy("Nano Swarm",       50,  7,  Track::NewMaterial));
    reg.registerMonster(Enemy("Carbon Golem",     120, 10, Track::NewMaterial));

    // Hydrogen
    reg.registerMonster(Enemy("Cryo Crawler",     55,  9,  Track::Hydrogen));
    reg.registerMonster(Enemy("Plasma Leech",     80,  11, Track::Hydrogen));

    // EcoTech
    reg.registerMonster(Enemy("Gust Wraith",      45,  8,  Track::EcoTech));
    reg.registerMonster(Enemy("Turbine Beast",    90,  13, Track::EcoTech));

    // AI
    reg.registerMonster(Enemy("Rogue Drone",      55,  9,  Track::AI));
    reg.registerMonster(Enemy("Neural Phantom",   95,  11, Track::AI));

    // Grid
    reg.registerMonster(Enemy("Static Fiend",     65,  8,  Track::Grid));
    reg.registerMonster(Enemy("Overload Titan",   130, 14, Track::Grid));
}
