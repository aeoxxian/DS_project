#ifndef EVENT_OUTCOME_H
#define EVENT_OUTCOME_H

#include <string>

// 이벤트 선택지의 결과 종류
enum class OutcomeType {
    HealParty,      // 아군 전체 HP 회복
    DamageParty,    // 아군 전체 HP 감소
    AddCard,        // 카드풀에 카드 추가 (랜덤 or 지정)
    RemoveCard,     // 카드풀에서 카드 제거
    GainGold,       // 골드 획득 (추후 상점 연계)
    LoseGold,       // 골드 손실
    Nothing,        // 아무 효과 없음
};

struct EventOutcome {
    OutcomeType type;
    int         value;   // 수치 (회복량, 데미지, 골드 등)
    std::string note;    // 결과 출력 문자열

    EventOutcome() : type(OutcomeType::Nothing), value(0) {}
    EventOutcome(OutcomeType t, int v, const std::string& n)
        : type(t), value(v), note(n) {}
};

#endif
