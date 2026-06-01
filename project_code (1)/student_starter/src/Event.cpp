#include "Event.h"
#include <iostream>

Event::Event() : id(0), choiceCount(0) {}

Event::Event(int id, const std::string& title, const std::string& description)
    : id(id), title(title), description(description), choiceCount(0) {}

bool Event::addChoice(const EventChoice& choice) {
    if (choiceCount >= MAX_CHOICES) return false;
    choices[choiceCount++] = choice;
    return true;
}

int         Event::getId() const          { return id; }
std::string Event::getTitle() const       { return title; }
std::string Event::getDescription() const { return description; }
int         Event::getChoiceCount() const { return choiceCount; }

const EventChoice& Event::getChoice(int index) const {
    static EventChoice empty;
    if (index < 0 || index >= choiceCount) return empty;
    return choices[index];
}

EventOutcome Event::run() const {
    std::cout << "\n=== " << title << " ===\n";
    std::cout << description << "\n\n";

    for (int i = 0; i < choiceCount; ++i)
        std::cout << "  [" << i << "] " << choices[i].text << "\n";

    std::cout << "> ";
    std::string line;
    std::getline(std::cin, line);
    int choice = (line.empty()) ? 0 : std::stoi(line);

    if (choice < 0 || choice >= choiceCount) choice = 0;

    std::cout << "\n" << choices[choice].outcome.note << "\n";
    return choices[choice].outcome;
}
