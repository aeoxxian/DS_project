#include "ds/CardRegistry.h"
#include "StatusEffect.h"   // 정식 상태이상 키 (ST_*)
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
// 이펙트 6종 (CardEffect{type, value, duration, stat, target}):
//   Attack  — value 데미지. 대상: 기본=첫 생존 적, AllEnemies=적 전체.
//   Defense — value 만큼 방어막(shield) 부여. 대상: 기본=시전자, Party=아군 전체. duration=지속턴.
//   Heal    — value 회복. 대상: 기본=시전자, Party=아군 전체. (대상의 burn 만큼 감소)
//   Buff    — 아군에게 상태 부여. 대상: 기본=시전자, Party=아군 전체.
//   Debuff  — 적에게 상태 부여. 대상: 기본=첫 생존 적, AllEnemies=적 전체.
//   Draw    — "다음 턴" 드로우 수를 value만큼 증가(1턴 지속, 즉시 뽑지 않음). 손패 최대 10장. target 무관.
//             손패는 매 턴 리셋되며, 기본 5장 + 직전 턴에 적립된 Draw 보너스만큼 뽑는다.
//
// 실제 작동하는 stat 키 (StatusEffect.h):
//   [Debuff/적] ST_POISON(매턴 피해), ST_ATK_DOWN(공격력↓), ST_DEF_DOWN(받는피해↑),
//               ST_WEAKEN(출력 %↓), ST_CONFUSE(확률 자해), ST_PARALYZE(1턴 행동불가), ST_SLEEP(3턴)
//   [Buff/아군] ST_ATK_UP(공격력↑), ST_DEF_UP(받는피해↓), ST_SHIELD(흡수막),
//               ST_EVADE(확률 회피), ST_BLOCK(1회 완전차단)
//   주의: ST_ATK_DOWN/DEF_DOWN/WEAKEN/EVADE/CONFUSE 등은 value를 "양수"로 (엔진이 부호 처리).
//
// addEffect()      — 기본 효과 (항상 발동)
// addBonusEffect() — 트랙 일치 시 추가 효과
//
// ── 카드 구성 ─────────────────────────────────────────────────────────────────
//   공용(ID 1~30)   : 트랙 무관 기본 카드. 원하는 트랙 카드가 안 나왔을 때의 토대.
//   트랙(ID 1xx~6xx): 각 20장. 트랙 캐릭터가 쓰면 bonus가 추가 발동하며 트랙별 색깔이 다름.
//     Nuclear     — 독(DoT) 중첩 + 고화력 폭발 (램프/리스크)
//     NewMaterial — 방어막/경감/블록, 파티 보호 (탱킹/지속)
//     Hydrogen    — 순간 화력 + 빙결(마비)로 템포 장악
//     EcoTech     — 회복/파티 유지 + 약화·회피 (지속/제어)
//     AI          — 카드 드로우 엔진 + 공격력 누적 (카드 어드밴티지)
//     Grid        — 전체 연쇄(AllEnemies) + 마비 + 한방 (광역/제어)

void registerAllCards() {
    CardRegistry& reg = CardRegistry::instance();

    // ══════════════════════════════════════════════════════════════
    //  공용 카드 (ID 1~30) — 보너스 없음, 모든 파티에 항상 등장
    // ══════════════════════════════════════════════════════════════
    { Card c(1,  "Strike",        "기본 공격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(2,  "Heavy Blow",    "강력한 일격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 10}); reg.registerCard(c); }
    { Card c(3,  "Precise Shot",  "정확한 사격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 7}); reg.registerCard(c); }
    { Card c(4,  "Cleave",        "적 전체를 벤다.", CardType::Normal);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(5,  "Reckless Swing","무모한 강타.", CardType::Normal);
      c.addEffect({EffectType::Attack, 13}); reg.registerCard(c); }
    { Card c(6,  "Guard",         "시전자 방어막.", CardType::Normal);
      c.addEffect({EffectType::Defense, 8, 1}); reg.registerCard(c); }
    { Card c(7,  "Brace",         "시전자 방어막(2턴).", CardType::Normal);
      c.addEffect({EffectType::Defense, 5, 2}); reg.registerCard(c); }
    { Card c(8,  "Bulwark",       "파티 전체 방어막.", CardType::Normal);
      c.addEffect({EffectType::Defense, 6, 2, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(9,  "Barrier",       "두꺼운 단발 방어막.", CardType::Normal);
      c.addEffect({EffectType::Defense, 12, 1}); reg.registerCard(c); }
    { Card c(10, "Mend",          "시전자 회복.", CardType::Normal);
      c.addEffect({EffectType::Heal, 6}); reg.registerCard(c); }
    { Card c(11, "Second Wind",   "시전자 대량 회복.", CardType::Normal);
      c.addEffect({EffectType::Heal, 10}); reg.registerCard(c); }
    { Card c(12, "First Aid",     "파티 전체 소량 회복.", CardType::Normal);
      c.addEffect({EffectType::Heal, 5, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(13, "Focus",         "시전자 공격력 강화.", CardType::Normal);
      c.addEffect({EffectType::Buff, 3, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(14, "War Cry",       "파티 공격력 강화.", CardType::Normal);
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP, EffectTarget::Party}); reg.registerCard(c); }
    { Card c(15, "Smoke Veil",    "시전자 회피율 상승.", CardType::Normal);
      c.addEffect({EffectType::Buff, 50, 2, ST_EVADE}); reg.registerCard(c); }
    { Card c(16, "Iron Stance",   "다음 공격 1회 완전 차단.", CardType::Normal);
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK}); reg.registerCard(c); }
    { Card c(17, "Quick Study",   "다음 턴 카드 2장 더 드로우.", CardType::Normal);
      c.addEffect({EffectType::Draw, 2}); reg.registerCard(c); }
    { Card c(18, "Insight",       "다음 턴 1장 더 드로우 + 소량 회복.", CardType::Normal);
      c.addEffect({EffectType::Draw, 1});
      c.addEffect({EffectType::Heal, 3}); reg.registerCard(c); }
    { Card c(19, "Adrenaline",    "다음 턴 2장 더 드로우 + 공격력 강화.", CardType::Normal);
      c.addEffect({EffectType::Draw, 2});
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(20, "Poison Dart",   "약한 독침.", CardType::Normal);
      c.addEffect({EffectType::Attack, 3});
      c.addEffect({EffectType::Debuff, 2, 2, ST_POISON}); reg.registerCard(c); }
    { Card c(21, "Disarm",        "적 공격력 감소.", CardType::Normal);
      c.addEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(22, "Expose",        "적 방어 약화.", CardType::Normal);
      c.addEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(23, "Concussion",    "타격 + 1턴 마비.", CardType::Normal);
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(24, "Weakening Blow","타격 + 약화.", CardType::Normal);
      c.addEffect({EffectType::Attack, 5});
      c.addEffect({EffectType::Debuff, 20, 2, ST_WEAKEN}); reg.registerCard(c); }
    { Card c(25, "Shield Bash",   "타격 + 방어막.", CardType::Normal);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Defense, 4, 1}); reg.registerCard(c); }
    { Card c(26, "Rally",         "파티 회복 + 공격력 강화.", CardType::Normal);
      c.addEffect({EffectType::Heal, 4, 0, "", EffectTarget::Party});
      c.addEffect({EffectType::Buff, 1, 2, ST_ATK_UP, EffectTarget::Party}); reg.registerCard(c); }
    { Card c(27, "Volley",        "적 전체 연사.", CardType::Normal);
      c.addEffect({EffectType::Attack, 4, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(28, "Counter Stance","1회 차단 + 공격력 강화.", CardType::Normal);
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK});
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(29, "Double Strike", "연속 2회 타격.", CardType::Normal);
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(30, "Last Stand",    "회복 + 방어막.", CardType::Normal);
      c.addEffect({EffectType::Heal, 8});
      c.addEffect({EffectType::Defense, 6, 1}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  Nuclear (ID 101~120) — 독(DoT) 중첩 + 고화력 폭발 (램프/리스크)
    // ══════════════════════════════════════════════════════════════
    { Card c(101, "Fission Burst", "핵분열 에너지 방출.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(102, "Meltdown", "불안정한 연쇄반응.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 12});
      c.addEffect({EffectType::Debuff, 2, 3, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(103, "Radiation Pulse", "방사선 범위 공격.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Debuff, 2, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(104, "Containment Breach", "방사능 누출로 적 전체 약화.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 7});
      c.addBonusEffect({EffectType::Debuff, 30, 2, ST_WEAKEN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(105, "Irradiate", "강한 독 주입.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Debuff, 3, 3, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 2, 3, ST_POISON}); reg.registerCard(c); }
    { Card c(106, "Fallout", "적 전체에 방사 낙진.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addEffect({EffectType::Debuff, 1, 2, ST_POISON, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 1, 2, ST_POISON, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(107, "Critical Mass", "임계 도달.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 15});
      c.addBonusEffect({EffectType::Debuff, 4, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(108, "Decay", "장기 붕괴 유도.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Debuff, 4, 4, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 3, 3, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(109, "Chain Reaction", "연쇄 폭발.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Debuff, 2, 2, ST_POISON});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(110, "Neutron Lance", "중성자 관통.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 11});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_POISON}); reg.registerCard(c); }
    { Card c(111, "Reactor Overload", "노심 과부하.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 9});
      c.addEffect({EffectType::Attack, 3});
      c.addBonusEffect({EffectType::Debuff, 2, 3, ST_POISON}); reg.registerCard(c); }
    { Card c(112, "Half-Life", "느리지만 끈질긴 붕괴.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Debuff, 2, 5, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 20, 2, ST_WEAKEN}); reg.registerCard(c); }
    { Card c(113, "Isotope Round", "방사성 탄두.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff, 2, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Debuff, 2, 2, ST_POISON}); reg.registerCard(c); }
    { Card c(114, "Atomic Slam", "원자 단위 강타.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 13});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(115, "Hazard Field", "적 전체에 오염 지대.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Debuff, 2, 3, ST_POISON, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 2, 2, ST_DEF_DOWN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(116, "Spent Fuel", "폐연료봉 투척.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Debuff, 4, 2, ST_POISON}); reg.registerCard(c); }
    { Card c(117, "Gamma Burst", "감마선 폭발.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 10});
      c.addBonusEffect({EffectType::Debuff, 25, 2, ST_WEAKEN}); reg.registerCard(c); }
    { Card c(118, "Cascade", "다단 핵분열.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Debuff, 1, 2, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 2, 2, ST_POISON}); reg.registerCard(c); }
    { Card c(119, "Core Breach", "노심 직격.", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 14});
      c.addEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Debuff, 3, 3, ST_POISON}); reg.registerCard(c); }
    { Card c(120, "Detonate", "전탄 기폭. (시그니처)", CardType::TrackCard, Track::Nuclear);
      c.addEffect({EffectType::Attack, 18});
      c.addBonusEffect({EffectType::Attack, 6}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  NewMaterial (ID 201~220) — 방어막/경감/블록, 파티 보호 (탱킹/지속)
    // ══════════════════════════════════════════════════════════════
    { Card c(201, "Nano Blade", "나노 날 베기.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Debuff, 3, 1, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(202, "Composite Shield", "경화 방어막.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 10, 1});
      c.addBonusEffect({EffectType::Buff, 3, 2, ST_DEF_UP}); reg.registerCard(c); }
    { Card c(203, "Polymer Strike", "유연한 타격 + 소량 회복.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Heal, 2});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(204, "Aegis Weave", "파티 방어막 + 방어 강화.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 5, 2, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_DEF_UP, EffectTarget::Party}); reg.registerCard(c); }
    { Card c(205, "Hardened Plating", "강화 장갑(2턴).", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 12, 2});
      c.addBonusEffect({EffectType::Buff, 1, 2, ST_BLOCK}); reg.registerCard(c); }
    { Card c(206, "Reactive Armor", "반응 장갑 반격.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 6, 1});
      c.addBonusEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(207, "Carbon Lattice", "탄소 격자 강화.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Buff, 4, 3, ST_DEF_UP});
      c.addBonusEffect({EffectType::Defense, 5, 2}); reg.registerCard(c); }
    { Card c(208, "Self-Repair", "자가 복원.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Heal, 8});
      c.addEffect({EffectType::Defense, 4, 1});
      c.addBonusEffect({EffectType::Heal, 4}); reg.registerCard(c); }
    { Card c(209, "Graphene Edge", "그래핀 절단.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 10});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(210, "Bastion", "파티 요새화(2턴).", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 8, 2, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_DEF_UP, EffectTarget::Party}); reg.registerCard(c); }
    { Card c(211, "Shock Absorber", "충격 흡수 자세.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK});
      c.addBonusEffect({EffectType::Defense, 5, 1}); reg.registerCard(c); }
    { Card c(212, "Memory Alloy", "형상기억 합금.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Heal, 6});
      c.addEffect({EffectType::Defense, 6, 1});
      c.addBonusEffect({EffectType::Buff, 1, 2, ST_BLOCK}); reg.registerCard(c); }
    { Card c(213, "Tensile Strike", "인장 강타.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Attack, 8});
      c.addEffect({EffectType::Defense, 5, 1});
      c.addBonusEffect({EffectType::Attack, 3}); reg.registerCard(c); }
    { Card c(214, "Ablative Shell", "소모형 장갑(대형).", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 15, 1});
      c.addBonusEffect({EffectType::Buff, 3, 2, ST_DEF_UP}); reg.registerCard(c); }
    { Card c(215, "Nanoweave Guard", "나노직 보호막.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 4, 2, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Heal, 3, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(216, "Resilience", "탄성 회복.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Buff, 5, 2, ST_DEF_UP});
      c.addBonusEffect({EffectType::Heal, 5}); reg.registerCard(c); }
    { Card c(217, "Counter Matrix", "반격 행렬.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK});
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(218, "Fortify", "진영 보강.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 7, 1});
      c.addEffect({EffectType::Buff, 2, 2, ST_DEF_UP});
      c.addBonusEffect({EffectType::Defense, 5, 1, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(219, "Adaptive Mesh", "적응형 메시 + 드로우.", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 6, 1});
      c.addEffect({EffectType::Draw, 1});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_DEF_UP}); reg.registerCard(c); }
    { Card c(220, "Impervious", "절대 방벽. (시그니처)", CardType::TrackCard, Track::NewMaterial);
      c.addEffect({EffectType::Defense, 10, 2, "", EffectTarget::Party});
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK});
      c.addBonusEffect({EffectType::Buff, 3, 2, ST_DEF_UP, EffectTarget::Party}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  Hydrogen (ID 301~320) — 순간 화력 + 빙결(마비)로 템포 장악
    // ══════════════════════════════════════════════════════════════
    { Card c(301, "Fuel Cell Blast", "수소 폭발.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 10});
      c.addBonusEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(302, "Cryo Jet", "극저온 스트림.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Debuff, 4, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(303, "Plasma Arc", "고온 수소 방전.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 11});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(304, "Disorient Mist", "신경 교란 가스.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Debuff, 40, 2, ST_CONFUSE}); reg.registerCard(c); }
    { Card c(305, "Flash Freeze", "순간 빙결.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Attack, 6}); reg.registerCard(c); }
    { Card c(306, "Hydrogen Bomb", "수소 폭탄.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 16});
      c.addBonusEffect({EffectType::Attack, 6}); reg.registerCard(c); }
    { Card c(307, "Frostbite", "동상 타격.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 8});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(308, "Pressure Spike", "압력 급상승.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(309, "Coolant Burst", "냉각재 분사(전체).", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 20, 2, ST_WEAKEN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(310, "Ignition", "점화 가속.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Buff, 3, 1, ST_ATK_UP});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(311, "Supercool", "초저온 제압.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addEffect({EffectType::Debuff, 20, 2, ST_WEAKEN});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(312, "Jet Stream", "연속 분사.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Attack, 6});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(313, "Cryo Lance", "빙결 창.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 12});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(314, "Vapor Slash", "기화 베기 + 회피.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Buff, 40, 1, ST_EVADE}); reg.registerCard(c); }
    { Card c(315, "Deep Freeze", "심층 동결(수면 3턴).", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Debuff, 3, 3, ST_SLEEP});
      c.addBonusEffect({EffectType::Debuff, 4, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(316, "Combustion", "연소 폭발.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 10});
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP});
      c.addBonusEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(317, "Liquid Nitrogen", "액체질소 분사.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Debuff, 30, 2, ST_WEAKEN}); reg.registerCard(c); }
    { Card c(318, "Overpressure", "과압 파열.", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 13});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(319, "Frost Nova", "빙결 폭발(전체).", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Attack, 3, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(320, "Zero Point", "절대영도 직격. (시그니처)", CardType::TrackCard, Track::Hydrogen);
      c.addEffect({EffectType::Attack, 14});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Attack, 6}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  EcoTech (ID 401~420) — 회복/파티 유지 + 약화·회피 (지속/제어)
    // ══════════════════════════════════════════════════════════════
    { Card c(401, "Gust Slash", "바람 강화 베기.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(402, "Vacuum Burst", "급격한 기압 하강.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(403, "Turbine Kick", "회전 타격.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Buff, 3, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(404, "Photosynthesis", "광합성 회복.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 8});
      c.addBonusEffect({EffectType::Heal, 4, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(405, "Tailwind", "순풍 가속.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP, EffectTarget::Party});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(406, "Solar Flare", "태양광 집속.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 7});
      c.addBonusEffect({EffectType::Debuff, 30, 2, ST_WEAKEN}); reg.registerCard(c); }
    { Card c(407, "Cleansing Rain", "정화의 비(파티 회복).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 5, 0, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Heal, 3, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(408, "Updraft", "상승 기류 회피.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Buff, 50, 2, ST_EVADE});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(409, "Verdant Growth", "초목 성장(파티).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 4, 0, "", EffectTarget::Party});
      c.addEffect({EffectType::Buff, 2, 2, ST_DEF_UP, EffectTarget::Party});
      c.addBonusEffect({EffectType::Heal, 3, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(410, "Hurricane", "태풍(전체 약화).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 25, 2, ST_WEAKEN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(411, "Zephyr Strike", "미풍 일격 + 회피.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Buff, 30, 1, ST_EVADE});
      c.addBonusEffect({EffectType::Attack, 3}); reg.registerCard(c); }
    { Card c(412, "Geothermal", "지열 일격 + 회복.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 6});
      c.addEffect({EffectType::Attack, 5});
      c.addBonusEffect({EffectType::Heal, 4}); reg.registerCard(c); }
    { Card c(413, "Pollinate", "수분 확산(파티 강화).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Buff, 3, 2, ST_ATK_UP, EffectTarget::Party});
      c.addBonusEffect({EffectType::Heal, 3, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(414, "Eye of Storm", "폭풍의 눈.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Buff, 60, 1, ST_EVADE});
      c.addEffect({EffectType::Heal, 4});
      c.addBonusEffect({EffectType::Buff, 1, 2, ST_BLOCK}); reg.registerCard(c); }
    { Card c(415, "Wind Shear", "윈드시어.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 8});
      c.addEffect({EffectType::Debuff, 20, 2, ST_WEAKEN});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(416, "Regrowth", "재생.", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 12});
      c.addBonusEffect({EffectType::Heal, 6, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(417, "Calm Winds", "잔잔한 바람(파티 회복 + 회피).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 5, 0, "", EffectTarget::Party});
      c.addEffect({EffectType::Buff, 30, 2, ST_EVADE});
      c.addBonusEffect({EffectType::Heal, 3, 0, "", EffectTarget::Party}); reg.registerCard(c); }
    { Card c(418, "Tornado", "토네이도(전체 제어).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Attack, 6, 0, "", EffectTarget::AllEnemies});
      c.addEffect({EffectType::Debuff, 20, 2, ST_WEAKEN, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(419, "Living Barrier", "생울타리(파티 보호).", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Defense, 5, 2, "", EffectTarget::Party});
      c.addEffect({EffectType::Heal, 4, 0, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_DEF_UP, EffectTarget::Party}); reg.registerCard(c); }
    { Card c(420, "Gaia's Blessing", "대지의 축복. (시그니처)", CardType::TrackCard, Track::EcoTech);
      c.addEffect({EffectType::Heal, 10, 0, "", EffectTarget::Party});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP, EffectTarget::Party}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  AI (ID 501~520) — 카드 드로우 엔진 + 공격력 누적 (카드 어드밴티지)
    // ══════════════════════════════════════════════════════════════
    { Card c(501, "Algorithm Strike", "계산된 타격.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 8});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(502, "Neural Surge", "시스템 과부하.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 10});
      c.addEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(503, "Data Wipe", "적 약화 + 자가 회복.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 5});
      c.addEffect({EffectType::Debuff, 4, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Heal, 4}); reg.registerCard(c); }
    { Card c(504, "Recompile", "재컴파일(드로우).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Draw, 2});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(505, "Optimize", "최적화 강화.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Buff, 3, 3, ST_ATK_UP});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(506, "Parallel Process", "병렬 처리.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Draw, 2});
      c.addEffect({EffectType::Attack, 4});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(507, "Overclock", "오버클럭.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Buff, 4, 2, ST_ATK_UP});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(508, "Predictive Aim", "예측 조준.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(509, "Cache Hit", "캐시 적중(드로우 + 회복).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Draw, 1});
      c.addEffect({EffectType::Heal, 3});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(510, "Logic Bomb", "논리 폭탄.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Debuff, 2, 2, ST_POISON});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(511, "Deep Learning", "심층 학습(대량 드로우).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Draw, 3});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(512, "Targeting Matrix", "조준 행렬.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP});
      c.addBonusEffect({EffectType::Attack, 3}); reg.registerCard(c); }
    { Card c(513, "Brute Force", "무차별 대입.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 12});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(514, "Defrag", "조각 모음(회복 + 드로우).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Heal, 6});
      c.addEffect({EffectType::Draw, 1});
      c.addBonusEffect({EffectType::Heal, 3}); reg.registerCard(c); }
    { Card c(515, "Encryption", "암호화 방벽 + 드로우.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Buff, 1, 2, ST_BLOCK});
      c.addEffect({EffectType::Draw, 1});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_DEF_UP}); reg.registerCard(c); }
    { Card c(516, "Neural Net", "신경망 분산 공격(전체).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(517, "Hotfix", "긴급 패치(회복).", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Heal, 8});
      c.addBonusEffect({EffectType::Draw, 1}); reg.registerCard(c); }
    { Card c(518, "Exploit", "취약점 공략.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 8});
      c.addEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(519, "Recursive Strike", "재귀 타격 + 드로우.", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Attack, 4});
      c.addEffect({EffectType::Draw, 1});
      c.addBonusEffect({EffectType::Attack, 3}); reg.registerCard(c); }
    { Card c(520, "Singularity", "특이점. (시그니처)", CardType::TrackCard, Track::AI);
      c.addEffect({EffectType::Attack, 10});
      c.addEffect({EffectType::Draw, 2});
      c.addBonusEffect({EffectType::Buff, 3, 2, ST_ATK_UP}); reg.registerCard(c); }

    // ══════════════════════════════════════════════════════════════
    //  Grid (ID 601~620) — 전체 연쇄(AllEnemies) + 마비 + 한방 (광역/제어)
    // ══════════════════════════════════════════════════════════════
    { Card c(601, "Power Surge", "그리드 과부하.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 11});
      c.addBonusEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(602, "Blackout Strike", "전력 차단.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Debuff, 4, 2, ST_ATK_DOWN}); reg.registerCard(c); }
    { Card c(603, "Arc Flash", "전기 방전.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 9});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(604, "Chain Lightning", "연쇄 번개(전체).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 6, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Attack, 3, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(605, "Overvoltage", "과전압.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 13});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(606, "Static Field", "정전기장(전체 마비).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 20, 2, ST_WEAKEN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(607, "Thunderclap", "뇌격(전체).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 8, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(608, "Conduit", "전도 강화.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 7});
      c.addEffect({EffectType::Buff, 3, 2, ST_ATK_UP});
      c.addBonusEffect({EffectType::Attack, 4}); reg.registerCard(c); }
    { Card c(609, "Surge Protector", "서지 방호.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Defense, 8, 1});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(610, "EMP", "전자기 펄스(전체 마비).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_ATK_DOWN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(611, "Tesla Coil", "테슬라 코일.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies});
      c.addEffect({EffectType::Attack, 5});
      c.addBonusEffect({EffectType::Attack, 3, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(612, "Voltage Spike", "전압 급등.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 10});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(613, "Grid Lock", "계통 마비(수면 3턴).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Debuff, 3, 3, ST_SLEEP});
      c.addBonusEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN}); reg.registerCard(c); }
    { Card c(614, "Discharge", "방전 일격.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 14});
      c.addBonusEffect({EffectType::Attack, 5}); reg.registerCard(c); }
    { Card c(615, "Capacitor", "축전 강화.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Buff, 4, 2, ST_ATK_UP});
      c.addBonusEffect({EffectType::Defense, 5, 1}); reg.registerCard(c); }
    { Card c(616, "Ball Lightning", "구전(전체 약화).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 7, 0, "", EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Debuff, 25, 2, ST_WEAKEN, EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(617, "Shock Therapy", "전기 충격.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 6});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE});
      c.addBonusEffect({EffectType::Buff, 2, 2, ST_ATK_UP}); reg.registerCard(c); }
    { Card c(618, "Power Grid", "전력망 분배(파티 강화).", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Buff, 2, 2, ST_ATK_UP, EffectTarget::Party});
      c.addBonusEffect({EffectType::Attack, 5, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
    { Card c(619, "Lightning Rod", "피뢰침.", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 9});
      c.addEffect({EffectType::Debuff, 3, 2, ST_DEF_DOWN});
      c.addBonusEffect({EffectType::Debuff, 1, 1, ST_PARALYZE}); reg.registerCard(c); }
    { Card c(620, "Storm Surge", "폭풍 해일(전체). (시그니처)", CardType::TrackCard, Track::Grid);
      c.addEffect({EffectType::Attack, 8, 0, "", EffectTarget::AllEnemies});
      c.addEffect({EffectType::Debuff, 1, 1, ST_PARALYZE, EffectTarget::AllEnemies});
      c.addBonusEffect({EffectType::Attack, 4, 0, "", EffectTarget::AllEnemies}); reg.registerCard(c); }
}
