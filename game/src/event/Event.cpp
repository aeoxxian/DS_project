#include "event/Event.h"
#include <iostream>

// ── EventChoice ───────────────────────────────────────────────────────────────

bool EventChoice::addOutcome(const EventOutcome& o) {
    if (outcomeCount >= MAX_OUTCOMES_PER_CHOICE) return false;
    outcomes[outcomeCount++] = o;
    return true;
}

int EventChoice::getOutcomeCount() const { return outcomeCount; }

const EventOutcome& EventChoice::getOutcome(int i) const {
    static EventOutcome empty;
    return (i >= 0 && i < outcomeCount) ? outcomes[i] : empty;
}

// ── Event ─────────────────────────────────────────────────────────────────────

Event::Event() : id(0), choiceCount(0), forced(false) {}

Event::Event(int id, const std::string& title, const std::string& description, bool forced)
    : id(id), title(title), description(description), choiceCount(0), forced(forced) {}

bool Event::addChoice(const EventChoice& choice) {
    if (choiceCount >= MAX_CHOICES) return false;
    choices[choiceCount++] = choice;
    return true;
}

int         Event::getId()          const { return id; }
std::string Event::getTitle()       const { return title; }
std::string Event::getDescription() const { return description; }
int         Event::getChoiceCount() const { return choiceCount; }
bool        Event::isForced()       const { return forced; }

const EventChoice& Event::getChoice(int i) const {
    static EventChoice empty;
    return (i >= 0 && i < choiceCount) ? choices[i] : empty;
}

int Event::run() const {
    std::cout << "\n";
    std::cout << "  ══════════════════════════════════════════════════\n";
    std::cout << "  이벤트: " << title << "\n";
    std::cout << "  ══════════════════════════════════════════════════\n";
    std::cout << "  " << description << "\n\n";

    if (forced || choiceCount <= 1) {
        std::cout << "  [ 강제 이벤트 — 자동 진행 ]\n";
        for (int i = 0; i < choices[0].getOutcomeCount(); ++i)
            std::cout << "  → " << choices[0].getOutcome(i).note << "\n";
        return 0;
    }

    for (int i = 0; i < choiceCount; ++i)
        std::cout << "  [" << i << "] " << choices[i].text << "\n";
    std::cout << "\n  선택 > ";

    std::string line;
    std::getline(std::cin, line);
    int choice = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? 0 : std::stoi(line);
    if (choice < 0 || choice >= choiceCount) choice = 0;

    const EventChoice& chosen = choices[choice];
    std::cout << "  → [" << chosen.text << "] 선택\n";
    for (int i = 0; i < chosen.getOutcomeCount(); ++i)
        std::cout << "  → " << chosen.getOutcome(i).note << "\n";

    return choice;
}
