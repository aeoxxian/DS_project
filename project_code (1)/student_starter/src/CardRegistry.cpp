#include "ds/CardRegistry.h"
#include <iostream>

CardRegistry::CardRegistry() : count(0) {}

CardRegistry& CardRegistry::instance() {
    static CardRegistry registry;
    return registry;
}

bool CardRegistry::registerCard(const Card& card) {
    if (count >= MAX_REGISTRY_SIZE) return false;
    for (int i = 0; i < count; ++i)
        if (entries[i].getId() == card.getId()) return false;
    entries[count++] = card;
    return true;
}

bool CardRegistry::findById(int id, Card& out) const {
    for (int i = 0; i < count; ++i)
        if (entries[i].getId() == id) { out = entries[i]; return true; }
    return false;
}

bool CardRegistry::findByName(const std::string& name, Card& out) const {
    for (int i = 0; i < count; ++i)
        if (entries[i].getName() == name) { out = entries[i]; return true; }
    return false;
}

bool CardRegistry::getAt(int index, Card& out) const {
    if (index < 0 || index >= count) return false;
    out = entries[index];
    return true;
}

int CardRegistry::size() const { return count; }

void CardRegistry::printAll() const {
    std::cout << "=== Card Registry (" << count << " cards) ===\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  [" << entries[i].getId() << "] ";
        entries[i].print();
        std::cout << "\n";
    }
}

// ── Built-in card definitions ─────────────────────────────────────────────────
// 이펙트 4종:
//   Attack  (value = 데미지)
//   Defense (value = 방어막 수치)
//   Heal    (value = 회복량)
//   Buff    (value > 0 버프 / value < 0 디버프, stat = "atk"/"def"/"poison"/"stun" 등)
//
// addEffect()      — 기본 효과 (항상 발동)
// addBonusEffect() — 트랙 일치 시 추가 효과

void registerAllCards() {
    CardRegistry& reg = CardRegistry::instance();

    // ── Normal ────────────────────────────────────────────────────
    { Card c(1, "Strike", "기본 공격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 5});
      reg.registerCard(c); }

    { Card c(2, "Heavy Blow", "강력한 일격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 10});
      reg.registerCard(c); }

    { Card c(3, "Guard", "방어막 생성.", CardType::Normal);
      c.addEffect({EffectType::Defense, 8});
      reg.registerCard(c); }

    { Card c(4, "Mend", "아군 회복.", CardType::Normal);
      c.addEffect({EffectType::Heal, 6});
      reg.registerCard(c); }

    // ── Nuclear ───────────────────────────────────────────────────
    { Card c(101, "Fission Burst", "핵분열 에너지 방출.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Debuff,3, 2, "def"}); // 적 방어력 감소
      reg.registerCard(c); }

    { Card c(102, "Meltdown", "불안정한 연쇄반응.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 12});
      c.addEffect({EffectType::Debuff,2, 3, "poison"});   // 독 3턴
      c.addBonusEffect({EffectType::Debuff,3, 2, "atk"}); // 적 공격력 감소
      reg.registerCard(c); }

    { Card c(103, "Radiation Pulse", "방사선 범위 공격.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Debuff,2, 2, "def"});
      reg.registerCard(c); }

    // ── NewMaterial ───────────────────────────────────────────────
    { Card c(201, "Nano Blade", "나노 날 베기.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Debuff,3, 1, "atk"});
      reg.registerCard(c); }

    { Card c(202, "Composite Shield", "경화 방어막.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 10});
      c.addBonusEffect({EffectType::Buff, 3, 2, "def"});  // 아군 방어력 증가
      reg.registerCard(c); }

    { Card c(203, "Polymer Strike", "유연한 타격 + 소량 회복.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Heal, 2});
      c.addBonusEffect({EffectType::Attack, 4});
      reg.registerCard(c); }

    // ── Hydrogen ──────────────────────────────────────────────────
    { Card c(301, "Fuel Cell Blast", "수소 폭발.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 10});
      c.addBonusEffect({EffectType::Attack, 5});
      reg.registerCard(c); }

    { Card c(302, "Cryo Jet", "극저온 스트림.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff,1, 1, "stun"});     // 1턴 스턴
      c.addBonusEffect({EffectType::Debuff,4, 2, "atk"});
      reg.registerCard(c); }

    { Card c(303, "Plasma Arc", "고온 수소 방전.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 11});
      c.addBonusEffect({EffectType::Debuff,4, 3, "poison"});
      reg.registerCard(c); }

    // ── EcoTech ────────────────────────────────────────────────
    { Card c(401, "Gust Slash", "바람 강화 베기.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Buff, 2, 2, "atk"});  // 아군 공격력 증가
      reg.registerCard(c); }

    { Card c(402, "Vacuum Burst", "급격한 기압 하강.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Debuff,3, 2, "def"});
      c.addBonusEffect({EffectType::Debuff,1, 1, "stun"});
      reg.registerCard(c); }

    { Card c(403, "Turbine Kick", "회전 타격.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Buff, 3, 2, "atk"});
      reg.registerCard(c); }

    // ── AI ────────────────────────────────────────────────────────
    { Card c(501, "Algorithm Strike", "계산된 타격.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Buff, 2, 2, "atk"});
      reg.registerCard(c); }

    { Card c(502, "Neural Surge", "시스템 과부하.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 10});
      c.addEffect({EffectType::Debuff,3, 2, "atk"});
      c.addBonusEffect({EffectType::Debuff,1, 1, "stun"});
      reg.registerCard(c); }

    { Card c(503, "Data Wipe", "적 버프 제거 + 피해.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 5});
      c.addEffect({EffectType::Debuff,4, 2, "def"});
      c.addBonusEffect({EffectType::Heal, 4});
      reg.registerCard(c); }

    // ── Grid ──────────────────────────────────────────────────────
    { Card c(601, "Power Surge", "그리드 과부하.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 11});
      c.addBonusEffect({EffectType::Attack, 5});
      reg.registerCard(c); }

    { Card c(602, "Blackout Strike", "전력 차단.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff,1, 1, "stun"});
      c.addBonusEffect({EffectType::Debuff,4, 2, "atk"});
      reg.registerCard(c); }

    { Card c(603, "Arc Flash", "전기 방전.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 9});
      c.addEffect({EffectType::Debuff,2, 3, "poison"});
      c.addBonusEffect({EffectType::Defense, 5});
      reg.registerCard(c); }
}
