#ifndef EVENT_REGISTRY_H
#define EVENT_REGISTRY_H

#include "event/Event.h"
#include "core/Constants.h"

class EventRegistry {
private:
    Event entries[MAX_EVENT_REGISTRY];
    int   count;
    EventRegistry();

public:
    static EventRegistry& instance();
    bool registerEvent(const Event& event);
    bool getAt(int index, Event& out) const;
    bool findById(int id, Event& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllEvents();

#endif
