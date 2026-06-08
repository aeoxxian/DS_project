#ifndef EFFECT_H
#define EFFECT_H

#include <string>

enum class EffectType {
    Attack,          // value + ATK - 대상 DEF
    HandScaleAttack, // 손패 수만큼 Attack 반복 (ATK 스케일)
    Defense,         // 방어막 (value, duration턴)
    Buff,            // 아군 강화 / 회복 (stat 키로 구분)
    Debuff,          // 적 상태이상
    Draw,            // 다음 턴 드로우 +value장
    Swap             // 자리 바꾸기 (플레이어: 선택, 적: 강제 랜덤)
};

enum class EffectTarget {
    Default,      // 공격 → 첫 번째 생존 적 / 버프·힐 → 시전자
    AllEnemies,   // 적 전체
    Party         // 아군 전체
};

struct Effect {
    EffectType   type;
    int          value;
    int          duration;
    std::string  stat;
    EffectTarget target;

    Effect() : type(EffectType::Attack), value(0), duration(0), target(EffectTarget::Default) {}
    Effect(EffectType t, int v, int d = 0,
           const std::string& s = "", EffectTarget tgt = EffectTarget::Default)
        : type(t), value(v), duration(d), stat(s), target(tgt) {}

    std::string toString() const;
};

// ── 상태이상 키 상수 ──────────────────────────────────────────────────────────
static const char* ST_POISON   = "poison";
static const char* ST_ATK_DOWN = "atk_down";
static const char* ST_DEF_DOWN = "def_down";
static const char* ST_WEAKEN   = "weaken";
static const char* ST_CONFUSE  = "confuse";
static const char* ST_PARALYZE = "stun";
static const char* ST_SLEEP    = "stun";    // stun으로 통일 (긴 duration)
static const char* ST_ATK_UP   = "atk_up";
static const char* ST_DEF_UP   = "def_up";
static const char* ST_EVADE    = "evade";
static const char* ST_BLOCK    = "block";

// ── 팩토리 ────────────────────────────────────────────────────────────────────
namespace Effects {
    inline Effect attack     (int v, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Attack,      v, 0, "", t); }
    inline Effect handScaleAttack(int v)
        { return Effect(EffectType::HandScaleAttack, v); }
    inline Effect defense    (int v, int d = 1, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Defense,     v, d, "", t); }
    inline Effect heal       (int v, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Buff,        v, 0, "heal", t); }
    inline Effect draw       (int v)
        { return Effect(EffectType::Draw,        v); }
    inline Effect swap       ()
        { return Effect(EffectType::Swap,        0); }

    inline Effect atkUp      (int v, int d) { return Effect(EffectType::Buff,   v, d, ST_ATK_UP); }
    inline Effect defUp      (int v, int d) { return Effect(EffectType::Buff,   v, d, ST_DEF_UP); }
    inline Effect evade      (int d)        { return Effect(EffectType::Buff,  50, d, ST_EVADE); }
    inline Effect block      (int d)        { return Effect(EffectType::Buff,   1, d, ST_BLOCK); }

    inline Effect poison     (int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, d,  d, ST_POISON,   t); }
    inline Effect weaken     (int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, 25, d, ST_WEAKEN,   t); }
    inline Effect atkDown    (int v, int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, v,  d, ST_ATK_DOWN, t); }
    inline Effect defDown    (int v, int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, v,  d, ST_DEF_DOWN, t); }
    inline Effect burn       (int v, int d) { return Effect(EffectType::Debuff, v, d, "burn"); }
    inline Effect confuse    (int v, int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, v,  d, ST_CONFUSE,  t); }
    inline Effect stun       (int d, EffectTarget t = EffectTarget::Default)
        { return Effect(EffectType::Debuff, 1,  d, ST_PARALYZE, t); }
    inline Effect vulnerable (int d)
        { return Effect(EffectType::Debuff, 50, d, "vulnerable"); }
}

#endif
