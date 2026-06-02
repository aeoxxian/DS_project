#include "CardEffect.h"

std::string CardEffect::toString() const {
    std::string s;
    switch (type) {
        case EffectType::Attack:  s = "ATK(" + std::to_string(value) + ")"; break;
        case EffectType::Defense: s = "DEF(" + std::to_string(value) + ")"; break;
        case EffectType::Heal:    s = "HEAL(" + std::to_string(value) + ")"; break;
        case EffectType::Buff:    s = "BUFF[" + stat + "+" + std::to_string(value) + "]"; break;
        case EffectType::Debuff:  s = "DEBUFF[" + stat + " " + std::to_string(value) + "]"; break;
        case EffectType::Draw:    s = "DRAW(" + std::to_string(value) + ")"; break;
    }
    if (duration > 0) s += "x" + std::to_string(duration) + "t";
    return s;
}
