#include "ds/EventRegistry.h"
#include <iostream>

EventRegistry::EventRegistry() : count(0) {}

EventRegistry& EventRegistry::instance() {
    static EventRegistry reg;
    return reg;
}

bool EventRegistry::registerEvent(const Event& event) {
    if (count >= MAX_EVENT_REGISTRY) return false;
    entries[count++] = event;
    return true;
}

bool EventRegistry::getAt(int index, Event& out) const {
    if (index < 0 || index >= count) return false;
    out = entries[index]; return true;
}

bool EventRegistry::findById(int id, Event& out) const {
    for (int i = 0; i < count; ++i)
        if (entries[i].getId() == id) { out = entries[i]; return true; }
    return false;
}

int  EventRegistry::size() const { return count; }

void EventRegistry::printAll() const {
    std::cout << "=== Event Registry (" << count << ") ===\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  [" << entries[i].getId() << "] " << entries[i].getTitle() << "\n";
}

// ── 이벤트 정의 ───────────────────────────────────────────────────────────────
// 이벤트 추가: Event 생성 → addChoice() → registerEvent()
// OutcomeType: HealParty / DamageParty / AddCard / RemoveCard / GainGold / LoseGold / Nothing

void registerAllEvents() {
    EventRegistry& reg = EventRegistry::instance();

    // TODO: 이벤트 내용 추가 예정
    // 예시 구조:
    //
    // Event e(1, "이벤트 제목", "이벤트 설명 텍스트");
    // e.addChoice(EventChoice("선택지 A", EventOutcome(OutcomeType::HealParty, 10, "체력을 회복했다.")));
    // e.addChoice(EventChoice("선택지 B", EventOutcome(OutcomeType::Nothing,   0,  "아무 일도 일어나지 않았다.")));
    // reg.registerEvent(e);

    (void)reg; // 내용 추가 전 경고 방지
}
