#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include "Constants.h"
#include <string>

// 상태이상 키 (Debuff)
constexpr const char* ST_WEAKEN   = "weaken";   // 가하는 데미지 % 감소
constexpr const char* ST_ATK_DOWN = "atk_down"; // 공격력 감소
constexpr const char* ST_MAG_DOWN = "mag_down"; // 마력 감소
constexpr const char* ST_DEF_DOWN = "def_down"; // 방어력 감소
constexpr const char* ST_MDEF_DOWN= "mdef_down";// 마법방어 감소
constexpr const char* ST_BURN     = "burn";      // 치유량 감소
constexpr const char* ST_CONFUSE  = "confuse";   // 확률로 본인 공격
constexpr const char* ST_PARALYZE = "paralyze";  // 1턴 행동 불가
constexpr const char* ST_SLEEP    = "sleep";     // 3턴 행동불능

// 상태이상 키 (Buff)
constexpr const char* ST_EVADE    = "evade";     // 확률로 공격 회피
constexpr const char* ST_BLOCK    = "block";     // 특정 공격 완전 차단
constexpr const char* ST_ATK_UP   = "atk_up";   // 공격력 증가
constexpr const char* ST_DEF_UP   = "def_up";   // 방어력 증가
constexpr const char* ST_SHIELD   = "shield";   // 방어막 (수치 차감)

struct ActiveStatus {
    std::string stat;
    int value;
    int duration;

    ActiveStatus() : value(0), duration(0) {}
    ActiveStatus(const std::string& s, int v, int d) : stat(s), value(v), duration(d) {}

    bool isExpired() const { return duration <= 0; }
    std::string toString() const;
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
    bool isStunned() const; // paralyze || sleep
    void removeDebuffs(int n); // 정화
    void tick();
    void print() const;
};

#endif
