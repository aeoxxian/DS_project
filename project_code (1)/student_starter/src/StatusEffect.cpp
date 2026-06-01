#include "StatusEffect.h"
#include <iostream>
#include <cstdlib>

std::string ActiveStatus::toString() const {
    std::string s = stat + "(" + std::to_string(value);
    if (duration > 0) s += "x" + std::to_string(duration) + "t";
    return s + ")";
}

StatusTracker::StatusTracker() : count(0) {}

void StatusTracker::apply(const std::string& stat, int value, int duration) {
    for (int i = 0; i < count; ++i) {
        if (statuses[i].stat == stat) {
            statuses[i].value = value; statuses[i].duration = duration;
            return;
        }
    }
    if (count < MAX_STATUS_EFFECTS)
        statuses[count++] = ActiveStatus(stat, value, duration);
}

int StatusTracker::getModifier(const std::string& stat) const {
    for (int i = 0; i < count; ++i)
        if (statuses[i].stat == stat) return statuses[i].value;
    return 0;
}

bool StatusTracker::has(const std::string& stat) const {
    for (int i = 0; i < count; ++i)
        if (statuses[i].stat == stat && statuses[i].duration > 0) return true;
    return false;
}

bool StatusTracker::isStunned() const {
    return has(ST_PARALYZE) || has(ST_SLEEP);
}

void StatusTracker::removeDebuffs(int n) {
    static const char* debuffs[] = {
        ST_WEAKEN, ST_ATK_DOWN, ST_MAG_DOWN, ST_DEF_DOWN,
        ST_MDEF_DOWN, ST_BURN, ST_CONFUSE, ST_PARALYZE, ST_SLEEP
    };
    int removed = 0;
    for (int attempt = 0; attempt < count * 3 && removed < n; ++attempt) {
        int idx = rand() % count;
        for (int d = 0; d < 9; ++d) {
            if (statuses[idx].stat == debuffs[d]) {
                for (int j = idx; j < count - 1; ++j) statuses[j] = statuses[j+1];
                --count; ++removed; break;
            }
        }
    }
    if (removed > 0) std::cout << "  (Cleansed " << removed << " debuff(s))\n";
}

void StatusTracker::tick() {
    int n = 0;
    for (int i = 0; i < count; ++i) {
        if (--statuses[i].duration > 0) statuses[n++] = statuses[i];
    }
    count = n;
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
