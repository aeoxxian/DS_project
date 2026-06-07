#include "card/Card.h"
#include <iostream>

Card::Card() : id(0), track(Track::None), targetScope(TargetScope::Single),
               effectCount(0), bonusEffectCount(0) {}

Card::Card(int id, const std::string& name, const std::string& desc,
           Track track, TargetScope scope)
    : id(id), name(name), description(desc), track(track), targetScope(scope),
      effectCount(0), bonusEffectCount(0) {}

int         Card::getId()          const { return id; }
std::string Card::getName()        const { return name; }
std::string Card::getDescription() const { return description; }
Track       Card::getTrack()       const { return track; }
bool        Card::isTrackCard()    const { return track != Track::None; }
TargetScope Card::getTargetScope() const { return targetScope; }

bool Card::addEffect(const Effect& e) {
    if (effectCount >= MAX_EFFECTS_PER_CARD) return false;
    effects[effectCount++] = e;
    return true;
}

bool Card::addBonusEffect(const Effect& e) {
    if (bonusEffectCount >= MAX_EFFECTS_PER_CARD) return false;
    bonusEffects[bonusEffectCount++] = e;
    return true;
}

int    Card::getEffectCount()      const { return effectCount; }
int    Card::getBonusEffectCount() const { return bonusEffectCount; }
Effect Card::getEffect(int i)      const { return (i >= 0 && i < effectCount) ? effects[i] : Effect(); }
Effect Card::getBonusEffect(int i) const { return (i >= 0 && i < bonusEffectCount) ? bonusEffects[i] : Effect(); }

void Card::print() const {
    std::cout << "[" << name << "]";
    if (track != Track::None) std::cout << " (" << trackToString(track) << ")";
    if (targetScope == TargetScope::All) std::cout << " ALL";
    for (int i = 0; i < effectCount; ++i) std::cout << " " << effects[i].toString();
    if (bonusEffectCount > 0) {
        std::cout << " +[";
        for (int i = 0; i < bonusEffectCount; ++i) std::cout << bonusEffects[i].toString();
        std::cout << "]";
    }
}
