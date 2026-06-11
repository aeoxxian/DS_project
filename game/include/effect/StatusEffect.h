#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include "core/Constants.h"
#include <string>

inline const char* statDisplayName(const std::string& stat) {
    if (stat == "poison")     return "독";
    if (stat == "weaken")     return "약화";
    if (stat == "vulnerable") return "취약";
    if (stat == "stun")       return "기절";
    if (stat == "confuse")    return "혼란";
    if (stat == "shield")     return "방어막";
    if (stat == "atk_up")     return "공격↑";
    if (stat == "def_up")     return "방어↑";
    if (stat == "evasion")    return "회피";
    if (stat == "block")      return "차단";
    if (stat == "burn")       return "화상";
    return stat.c_str();
}

struct ActiveStatus {
    std::string stat;
    int value;
    int duration;

    ActiveStatus() : value(0), duration(0) {}
    ActiveStatus(const std::string& s, int v, int d) : stat(s), value(v), duration(d) {}

    bool isExpired() const { return duration <= 0; }
    std::string toString() const;
};

struct StatusResult {
    bool canAct;
    bool attacksSelf;
    int  poisonDamage;
    int  healReduction;

    StatusResult() : canAct(true), attacksSelf(false), poisonDamage(0), healReduction(0) {}
};

class StatusTracker {
private:
    ActiveStatus statuses[MAX_STATUS_EFFECTS];
    int count;

public:
    StatusTracker();

    void apply(const std::string& stat, int value, int duration);
    int  getModifier(const std::string& stat) const;
    bool has(const std::string& stat) const;
    bool isStunned() const;
    void removeDebuffs(int n);
    void clear();
    void tick();
    void        print()    const;
    std::string toString() const;

    StatusResult processTurn(const std::string& name);
};

#endif
