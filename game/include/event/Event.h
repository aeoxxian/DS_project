#ifndef EVENT_H
#define EVENT_H

#include "core/Constants.h"
#include <string>

enum class OutcomeType {
    HealParty, DamageParty, AddCard, RemoveCard,
    GainGold, LoseGold, Nothing, HealRest, UpgradeCard,
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
    EventOutcome outcome;

    EventChoice() {}
    EventChoice(const std::string& t, const EventOutcome& o) : text(t), outcome(o) {}
};

class Event {
private:
    int         id;
    std::string title;
    std::string description;
    EventChoice choices[MAX_CHOICES];
    int         choiceCount;

public:
    Event();
    Event(int id, const std::string& title, const std::string& description);

    bool addChoice(const EventChoice& choice);

    int                getId()          const;
    std::string        getTitle()       const;
    std::string        getDescription() const;
    int                getChoiceCount() const;
    const EventChoice& getChoice(int i) const;

    EventOutcome run() const;
};

Event makeRestEvent();

#endif
