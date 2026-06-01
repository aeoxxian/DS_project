#ifndef EVENT_REGISTRY_H
#define EVENT_REGISTRY_H

#include "Event.h"
#include "Constants.h"

class EventRegistry {
private:
    Event entries[MAX_EVENT_REGISTRY];
    int   count;
    EventRegistry();

public:
    static EventRegistry& instance();

    bool  registerEvent(const Event& event);
    bool  getAt(int index, Event& out) const;
    bool  findById(int id, Event& out) const;
    int   size() const;
    void  printAll() const;
};

// 게임 시작 시 한 번 호출 — 이벤트 내용은 여기에만 추가
void registerAllEvents();

#endif
