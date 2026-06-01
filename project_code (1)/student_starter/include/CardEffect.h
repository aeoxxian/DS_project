#ifndef CARD_EFFECT_H
#define CARD_EFFECT_H

#include <string>

enum class EffectType { Attack, Defense, Heal, Buff, Debuff };

struct CardEffect {
    EffectType  type;
    int         value;    // 데미지 / 회복량 / 방어막 / 스탯 변화량
    int         duration; // 지속 턴 (0 = 즉발)
    std::string stat;     // Buff/Debuff 대상 키

    CardEffect() : type(EffectType::Attack), value(0), duration(0) {}
    CardEffect(EffectType t, int v, int d = 0, const std::string& s = "")
        : type(t), value(v), duration(d), stat(s) {}

    std::string toString() const;
};

#endif
