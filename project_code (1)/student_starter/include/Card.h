#ifndef CARD_H
#define CARD_H

#include "Constants.h"
#include "Track.h"
#include "CardEffect.h"

enum class CardType { Normal, TrackCard };

class Card {
private:
    int id;
    std::string name;
    std::string description;
    CardType type;
    Track    track;
    CardEffect effects[MAX_EFFECTS_PER_CARD];
    int        effectCount;
    CardEffect bonusEffects[MAX_EFFECTS_PER_CARD];
    int        bonusEffectCount;

public:
    Card();
    Card(int id, const std::string& name, const std::string& desc,
         CardType type, Track track = Track::None);

    int         getId() const;
    std::string getName() const;
    std::string getDescription() const;
    CardType    getType() const;
    Track       getTrack() const;

    bool       addEffect(const CardEffect& e);
    bool       addBonusEffect(const CardEffect& e);
    int        getEffectCount() const;
    int        getBonusEffectCount() const;
    CardEffect getEffect(int i) const;
    CardEffect getBonusEffect(int i) const;

    void print() const;
};

#endif
