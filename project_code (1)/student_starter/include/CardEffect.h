#ifndef CARD_EFFECT_H
#define CARD_EFFECT_H

#include <string>

enum class EffectType { Attack, Defense, Heal, Buff, Debuff, Draw };
//  Draw — "다음 턴" 드로우 수를 value만큼 증가(1턴 지속). 즉시 뽑지 않음. 손패 최대 10장. target 무관.

// 효과 대상(scope).
//  Default = 타입별 기본값: Attack/Debuff -> 첫 생존 적, Heal/Defense/Buff -> 시전자 본인
//  Self/Party = 아군 본인 / 아군 전체, Enemy/AllEnemies = 첫 생존 적 / 적 전체
enum class EffectTarget { Default, Self, Party, Enemy, AllEnemies };

struct CardEffect {
    EffectType   type;
    int          value;    // 데미지 / 회복량 / 방어막 / 스탯 변화량
    int          duration; // 지속 턴 (0 = 즉발)
    std::string  stat;     // Buff/Debuff 대상 키
    EffectTarget target;   // 효과 대상 (생략 시 Default)

    CardEffect()
        : type(EffectType::Attack), value(0), duration(0), target(EffectTarget::Default) {}
    CardEffect(EffectType t, int v, int d = 0, const std::string& s = "",
               EffectTarget tg = EffectTarget::Default)
        : type(t), value(v), duration(d), stat(s), target(tg) {}

    std::string toString() const;
};

#endif
