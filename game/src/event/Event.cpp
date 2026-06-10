#include "event/Event.h"
#include "core/UI.h"
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
    static const int W = 56;
    std::cout << "\n";
    UI::boxTop(W);
    UI::boxCenter("!  이벤트  !", W);
    UI::boxMid(W);
    UI::boxCenter(title, W);
    UI::boxDiv(W);
    UI::boxEmpty(W);
    UI::boxLeft(description, W);
    UI::boxEmpty(W);

    if (forced || choiceCount <= 1) {
        UI::boxDiv(W);
        UI::boxCenter("[ 자동 진행 ]", W);
        UI::boxEmpty(W);
        for (int i = 0; i < choices[0].getOutcomeCount(); ++i)
            UI::boxLeft("→  " + choices[0].getOutcome(i).note, W);
        UI::boxEmpty(W);
        UI::boxBot(W);
        std::cout << "\n";
        return 0;
    }

    UI::boxDiv(W);
    UI::boxEmpty(W);
    for (int i = 0; i < choiceCount; ++i)
        UI::boxLeft("[" + std::to_string(i) + "]  " + choices[i].text, W);
    UI::boxEmpty(W);
    UI::boxBot(W);

    std::cout << "\n  선택 > ";
    std::string line;
    std::getline(std::cin, line);
    int choice = (line.empty() || !(line[0] >= '0' && line[0] <= '9')) ? 0 : std::stoi(line);
    if (choice < 0 || choice >= choiceCount) choice = 0;

    const EventChoice& chosen = choices[choice];
    std::cout << "\n";
    UI::typewrite("▷  [" + chosen.text + "]", 15);
    for (int i = 0; i < chosen.getOutcomeCount(); ++i)
        UI::typewrite("→  " + chosen.getOutcome(i).note, 15);
    std::cout << "\n";

    return choice;
}
