#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include "Constants.h"
#include <string>

// 상태이상 키 (Debuff)
constexpr const char* ST_POISON   = "poison";   // 매턴 value 데미지 (DoT)
constexpr const char* ST_WEAKEN   = "weaken";   // 가하는 데미지 % 감소 (value%)
constexpr const char* ST_ATK_DOWN = "atk_down"; // 공격력 감소 (출력 데미지 -value)
constexpr const char* ST_DEF_DOWN = "def_down"; // 방어력 감소 (받는 데미지 +value)
constexpr const char* ST_BURN     = "burn";      // 치유량 감소 (받는 회복 -value)
constexpr const char* ST_CONFUSE  = "confuse";   // value% 확률로 본인 공격(자해)
constexpr const char* ST_PARALYZE = "paralyze";  // 1턴 행동 불가
constexpr const char* ST_SLEEP    = "sleep";     // 3턴 행동불능

// 상태이상 키 (Buff)
constexpr const char* ST_EVADE    = "evade";     // value% 확률로 공격 회피
constexpr const char* ST_BLOCK    = "block";     // 다음 1회 공격 완전 차단(소모)
constexpr const char* ST_ATK_UP   = "atk_up";   // 공격력 증가 (출력 데미지 +value)
constexpr const char* ST_DEF_UP   = "def_up";   // 방어력 증가 (받는 데미지 -value)
constexpr const char* ST_SHIELD   = "shield";   // 방어막 흡수 풀 (받는 데미지 -value, 소모성)

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
    void consume(const std::string& stat, int amount); // 값 차감, 0 이하면 제거 (shield 흡수)
    void remove(const std::string& stat);              // 특정 상태 즉시 제거 (block 1회 소모)
    void removeDebuffs(int n); // 정화
    void tick();
    void print() const;
};

#endif
