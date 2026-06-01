#ifndef EVENT_H
#define EVENT_H

#include "EventOutcome.h"
#include "Constants.h"

// 이벤트 선택지 하나
struct EventChoice {
    std::string   text;     // 선택지 표시 문자열
    EventOutcome  outcome;  // 선택 시 발생하는 결과

    EventChoice() {}
    EventChoice(const std::string& text, const EventOutcome& outcome)
        : text(text), outcome(outcome) {}
};

// 이벤트 하나 — 제목, 설명, 선택지 목록
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

    int         getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    int         getChoiceCount() const;
    const EventChoice& getChoice(int index) const;

    // 이벤트 출력 및 선택지 입력. 선택된 Outcome 반환.
    EventOutcome run() const;
};

#endif
