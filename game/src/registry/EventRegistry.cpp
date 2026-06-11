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

// ── 헬퍼 ─────────────────────────────────────────────────────────────────────

static EventOutcome heal(int v)   { return {OutcomeType::HealParty,        v, "파티 전체 HP +" + std::to_string(v) + " 회복"}; }
static EventOutcome dmg(int v)    { return {OutcomeType::DamageParty,       v, "파티 전체 피해 " + std::to_string(v)}; }
static EventOutcome card()        { return {OutcomeType::AddCard,           1, "무작위 카드 1장 획득"}; }
static EventOutcome removeCard()  { return {OutcomeType::RemoveSelectedCard, 0, "카드 1장 제거 (선택)"}; }
static EventOutcome shop()        { return {OutcomeType::OpenShop,           0, "수상한 상점 오픈"}; }

// ── 이벤트 정의 ───────────────────────────────────────────────────────────────

void registerAllEvents() {
    EventRegistry& reg = EventRegistry::instance();

    // 1. 갑작스러운 정전 (강제)
    {
        Event e(1, "갑작스러운 정전",
                "시설 전체의 조명이 꺼지고, 어둠 속을 더듬어 이동합니다.", true);
        EventChoice eff;
        eff.addOutcome(dmg(5));
        e.addChoice(eff);
        reg.registerEvent(e);
    }

    // 2. 대학원생의 연구노트 (선택)
    {
        Event e(2, "대학원생의 연구노트",
                "연구노트를 읽고 어떤 내용을 확인할지 고릅니다.");
        EventChoice c1("핵심 요약 부분을 읽는다");
        c1.addOutcome(card());
        e.addChoice(c1);
        EventChoice c2("위험 표시가 붙은 실험식을 시도한다 [카드 획득 + 피해 4]");
        c2.addOutcome(card());
        c2.addOutcome(dmg(4));
        e.addChoice(c2);
        reg.registerEvent(e);
    }

    // 3. 교수님의 노트북 (강제)
    {
        Event e(3, "교수님의 노트북",
                "노트북의 덱 최적화 프로그램을 실행합니다.", true);
        EventChoice eff;
        eff.addOutcome(removeCard());
        e.addChoice(eff);
        reg.registerEvent(e);
    }

    // 4. 도서관 (선택)
    {
        Event e(4, "도서관",
                "도서관에서 어떤 책을 읽을지 선택합니다.");
        EventChoice c1("전공서적을 읽는다");
        c1.addOutcome(card());
        e.addChoice(c1);
        EventChoice c2("문학책을 읽는다");
        c2.addOutcome(heal(10));
        e.addChoice(c2);
        EventChoice c3("전공서적과 문학책을 모두 읽는다 [카드 획득 + 피해 5]");
        c3.addOutcome(card());
        c3.addOutcome(dmg(5));
        e.addChoice(c3);
        reg.registerEvent(e);
    }

    // 5. 수상한 용액 (선택)
    {
        Event e(5, "수상한 용액",
                "실험대 위의 정체불명 용액을 어떻게 할지 고릅니다.");
        EventChoice c1("용액을 떨어뜨려 본다 [파티 피해 8]");
        c1.addOutcome(dmg(8));
        e.addChoice(c1);
        EventChoice c2("직접 마셔본다 [파티 HP +15]");
        c2.addOutcome(heal(15));
        e.addChoice(c2);
        reg.registerEvent(e);
    }

    // 6. 수상한 상점 (강제)
    {
        Event e(6, "수상한 상점",
                "전리품을 사용해서 보상을 고를 수 있는 상점이 열립니다.", true);
        EventChoice eff;
        eff.addOutcome(shop());
        e.addChoice(eff);
        reg.registerEvent(e);
    }
}
