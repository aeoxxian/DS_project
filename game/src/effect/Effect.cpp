#include "effect/Effect.h"

std::string Effect::toString() const {
    std::string s;
    switch (type) {
        case EffectType::Attack:          s = "ATK(" + std::to_string(value) + ")"; break;
        case EffectType::HandScaleAttack: s = "HAND*" + std::to_string(value); break;
        case EffectType::Defense:         s = "DEF("  + std::to_string(value) + ")"; break;
        case EffectType::Buff:            s = "BUFF["  + stat + "+" + std::to_string(value) + "]"; break;
        case EffectType::Debuff:          s = "DEBUFF[" + stat + " " + std::to_string(value) + "]"; break;
        case EffectType::Draw:            s = "DRAW(" + std::to_string(value) + ")"; break;
        case EffectType::Swap:            s = "SWAP"; break;
    }
    if (duration > 0) s += "x" + std::to_string(duration) + "t";
    return s;
}
