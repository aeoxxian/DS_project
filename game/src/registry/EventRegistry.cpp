#include "registry/EventRegistry.h"
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

int EventRegistry::size() const { return count; }

void EventRegistry::printAll() const {
    std::cout << "=== Event Registry (" << count << ") ===\n";
    for (int i = 0; i < count; ++i)
        std::cout << "  [" << entries[i].getId() << "] " << entries[i].getTitle() << "\n";
}

// ── 이벤트 정의 ───────────────────────────────────────────────────────────────
// Event e(id, "제목", "설명");
// e.addChoice(EventChoice("선택지", EventOutcome(OutcomeType::HealParty, 10, "회복했다.")));
// reg.registerEvent(e);

void registerAllEvents() {
    EventRegistry& reg = EventRegistry::instance();
    // TODO: 이벤트 추가
    (void)reg;
}
