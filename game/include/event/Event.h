#ifndef EVENT_H
#define EVENT_H

#include "core/Constants.h"
#include <string>

enum class OutcomeType {
    HealParty, DamageParty, AddCard, RemoveCard, RemoveSelectedCard,
    GainGold, LoseGold, Nothing, HealRest, UpgradeCard, OpenShop
};

struct EventOutcome {
    OutcomeType type;
    int         value;
    std::string note;

    EventOutcome() : type(OutcomeType::Nothing), value(0) {}
    EventOutcome(OutcomeType t, int v, const std::string& n) : type(t), value(v), note(n) {}
};

struct EventChoice {
    std::string  text;
    EventOutcome outcomes[MAX_OUTCOMES_PER_CHOICE];
    int          outcomeCount;

    EventChoice() : outcomeCount(0) {}
    explicit EventChoice(const std::string& t) : text(t), outcomeCount(0) {}

    bool             addOutcome(const EventOutcome& o);
    int              getOutcomeCount()   const;
    const EventOutcome& getOutcome(int i) const;
};

class Event {
private:
    int         id;
    std::string title;
    std::string description;
    EventChoice choices[MAX_CHOICES];
    int         choiceCount;
    bool        forced;

public:
    Event();
    Event(int id, const std::string& title, const std::string& description, bool forced = false);

    bool addChoice(const EventChoice& choice);

    int                getId()          const;
    std::string        getTitle()       const;
    std::string        getDescription() const;
    int                getChoiceCount() const;
    const EventChoice& getChoice(int i) const;
    bool               isForced()       const;

    int run() const;  // returns chosen index (0 for forced events)
};

#endif
