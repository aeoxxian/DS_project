#include "Card.h"
#include <iostream>

Card::Card()
    : id(0), name(""), description(""), type(CardType::Normal),
      track(Track::None), effectCount(0), bonusEffectCount(0) {}

Card::Card(int id, const std::string& name, const std::string& description,
           CardType type, Track track)
    : id(id), name(name), description(description), type(type),
      track(track), effectCount(0), bonusEffectCount(0) {}

int Card::getId() const { return id; }
std::string Card::getName() const { return name; }
std::string Card::getDescription() const { return description; }
CardType Card::getType() const { return type; }
Track Card::getTrack() const { return track; }

bool Card::addEffect(const CardEffect& effect) {
    if (effectCount >= MAX_EFFECTS_PER_CARD) return false;
    effects[effectCount++] = effect;
    return true;
}

bool Card::addBonusEffect(const CardEffect& effect) {
    if (bonusEffectCount >= MAX_EFFECTS_PER_CARD) return false;
    bonusEffects[bonusEffectCount++] = effect;
    return true;
}

int Card::getEffectCount() const { return effectCount; }
int Card::getBonusEffectCount() const { return bonusEffectCount; }

CardEffect Card::getEffect(int index) const {
    if (index < 0 || index >= effectCount) return CardEffect();
    return effects[index];
}

CardEffect Card::getBonusEffect(int index) const {
    if (index < 0 || index >= bonusEffectCount) return CardEffect();
    return bonusEffects[index];
}

void Card::print() const {
    std::cout << "[" << name << "]";
    if (type == CardType::TrackCard)
        std::cout << " (" << trackToString(track) << ")";

    std::cout << " | ";
    for (int i = 0; i < effectCount; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << effects[i].toString();
    }
    if (bonusEffectCount > 0) {
        std::cout << " | Bonus: ";
        for (int i = 0; i < bonusEffectCount; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << bonusEffects[i].toString();
        }
    }
    std::cout << " | " << description;
}
