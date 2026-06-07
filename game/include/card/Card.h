#ifndef CARD_H
#define CARD_H

#include "core/Constants.h"
#include "core/Track.h"
#include "effect/Effect.h"

enum class TargetScope { Single, All };

class Card {
private:
    int         id;
    std::string name;
    std::string description;
    Track       track;
    TargetScope targetScope;
    Effect      effects[MAX_EFFECTS_PER_CARD];
    int         effectCount;
    Effect      bonusEffects[MAX_EFFECTS_PER_CARD];
    int         bonusEffectCount;

public:
    Card();
    Card(int id, const std::string& name, const std::string& desc,
         Track track = Track::None, TargetScope scope = TargetScope::Single);

    int         getId()          const;
    std::string getName()        const;
    std::string getDescription() const;
    Track       getTrack()       const;
    bool        isTrackCard()    const;
    TargetScope getTargetScope() const;

    bool   addEffect(const Effect& e);
    bool   addBonusEffect(const Effect& e);
    int    getEffectCount()      const;
    int    getBonusEffectCount() const;
    Effect getEffect(int i)      const;
    Effect getBonusEffect(int i) const;

    void print() const;
};

#endif
