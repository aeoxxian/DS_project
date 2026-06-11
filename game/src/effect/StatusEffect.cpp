#include "effect/StatusEffect.h"
#include <iostream>
#include <cstdlib>

static const int CONFUSE_CHANCE = 30;

std::string ActiveStatus::toString() const {
    std::string kname = statDisplayName(stat);
    std::string s = kname + "(" + std::to_string(value);
    if (duration > 0) s += "x" + std::to_string(duration) + "턴";
    return s + ")";
}

StatusTracker::StatusTracker() : count(0) {}

void StatusTracker::apply(const std::string& stat, int value, int duration) {
    if (stat == "poison") {
        for (int i = 0; i < count; ++i) {
            if (statuses[i].stat == "poison") {
                statuses[i].value    += value;
                statuses[i].duration += duration;
                return;
            }
        }
    }
    if (count < MAX_STATUS_EFFECTS)
        statuses[count++] = ActiveStatus(stat, value, duration);
}

int StatusTracker::getModifier(const std::string& stat) const {
    int total = 0;
    for (int i = 0; i < count; ++i)
        if (statuses[i].stat == stat) total += statuses[i].value;
    return total;
}

bool StatusTracker::has(const std::string& stat) const {
    for (int i = 0; i < count; ++i)
        if (statuses[i].stat == stat && statuses[i].duration > 0) return true;
    return false;
}

bool StatusTracker::isStunned() const { return has("stun"); }

void StatusTracker::removeDebuffs(int n) {
    static const char* debuffs[] = {
        "weaken","vulnerable","atk_down","mag_down","def_down",
        "burn","confuse","stun"
    };
    int removed = 0;
    for (int attempt = 0; attempt < count * 3 && removed < n; ++attempt) {
        if (count == 0) break;
        int idx = rand() % count;
        for (int d = 0; d < 7; ++d) {
            if (statuses[idx].stat == debuffs[d]) {
                for (int j = idx; j < count - 1; ++j) statuses[j] = statuses[j+1];
                --count; ++removed; break;
            }
        }
    }
    if (removed > 0) std::cout << "  (" << removed << "개 디버프 제거)\n";
}

void StatusTracker::clear() { count = 0; }

void StatusTracker::tick() {
    int n = 0;
    for (int i = 0; i < count; ++i)
        if (--statuses[i].duration > 0) statuses[n++] = statuses[i];
    count = n;
}

std::string StatusTracker::toString() const {
    if (!count) return "";
    std::string s = " [";
    for (int i = 0; i < count; ++i) {
        if (i) s += " ";
        s += statuses[i].toString();
    }
    return s + "]";
}

void StatusTracker::print() const {
    if (!count) return;
    std::cout << " [";
    for (int i = 0; i < count; ++i) {
        if (i) std::cout << " ";
        std::cout << statuses[i].toString();
    }
    std::cout << "]";
}

StatusResult StatusTracker::processTurn(const std::string& name) {
    StatusResult result;

    if (has("stun")) {
        std::cout << "  " << name << " 기절 상태!\n";
        result.canAct = false;
    }

    if (result.canAct && has("confuse") && rand() % 100 < CONFUSE_CHANCE) {
        std::cout << "  " << name << " 혼란 상태!\n";
        result.attacksSelf = true;
    }

    for (int i = 0; i < count; ++i) {
        if (statuses[i].stat == "poison" && statuses[i].duration > 0)
            result.poisonDamage += statuses[i].value;
    }
    if (result.poisonDamage > 0)
        std::cout << "  " << name << " 독 데미지 -" << result.poisonDamage << "\n";

    if (has("burn")) {
        result.healReduction = 50;
        std::cout << "  " << name << " 화상! (회복량 50% 감소)\n";
    }

    return result;
}
