#include "registry/CardRegistry.h"
#include "effect/Effect.h"
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
    out = entries[index]; return true;
}

int  CardRegistry::size() const { return count; }

void CardRegistry::printAll() const {
    std::cout << "=== Card Registry (" << count << " cards) ===\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  [" << entries[i].getId() << "] ";
        entries[i].print(); std::cout << "\n";
    }
}

void registerAllCards() {
    using namespace Effects;
    CardRegistry& reg = CardRegistry::instance();

    // ── 공용 (ID 1~30) ────────────────────────────────────────────
    { Card c(1,  "타격",         " 5 데미지를 가합니다.");
      c.addEffect(attack(5)); reg.registerCard(c); }
    { Card c(2,  "강타",         "강한 일격으로  10 데미지를 가합니다.");
      c.addEffect(attack(10)); reg.registerCard(c); }
    { Card c(3,  "정밀 사격",     "정확하게 조준해  7 데미지를 가합니다.");
      c.addEffect(attack(7)); reg.registerCard(c); }
    { Card c(4,  "베어가르기",    "전체 적에게  5 데미지를 가합니다.");
      c.addEffect(attack(5, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(5,  "무모한 휘두르기","무모하게 휘둘러  13 데미지를 가합니다.");
      c.addEffect(attack(13)); reg.registerCard(c); }
    { Card c(6,  "수비",         "방어막 8을 생성합니다.(1턴)");
      c.addEffect(defense(8, 1)); reg.registerCard(c); }
    { Card c(7,  "방어 태세",     "자세를 낮춰 방어막 5를 생성합니다.(1턴)");
      c.addEffect(defense(5, 1)); reg.registerCard(c); }
    { Card c(8,  "방벽",         "파티 전체에 방어막 6을 부여합니다.(1턴)");
      c.addEffect(defense(6, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(9,  "장벽",         "두꺼운 방어막 12를 생성합니다.(1턴)");
      c.addEffect(defense(12, 1)); reg.registerCard(c); }
    { Card c(10, "치유",         "상처를 치유해 HP를 6 회복합니다.");
      c.addEffect(heal(6)); reg.registerCard(c); }
    { Card c(11, "숨 고르기",     "숨을 가다듬어 HP를 10 회복합니다.");
      c.addEffect(heal(10)); reg.registerCard(c); }
    { Card c(12, "응급 처치",     "파티 전체의 HP를 5 회복합니다.");
      c.addEffect(heal(5, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(13, "집중",         "집중력을 높여 공격력을 3 증가시킵니다.(1턴)");
      c.addEffect(atkUp(3, 1)); reg.registerCard(c); }
    { Card c(14, "전투 함성",     "파티 전체의 공격력을 2 증가시킵니다.(1턴)");
      c.addEffect(Effect(EffectType::Buff, 2, 1, ST_ATK_UP, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(15, "연막",         "연막을 펼쳐 회피율을 50% 높입니다.(1턴)");
      c.addEffect(evade(1)); reg.registerCard(c); }
    { Card c(16, "철벽 태세",     "철벽 자세로 방어막 15를 생성합니다.(1턴)");
      c.addEffect(defense(15, 1)); reg.registerCard(c); }
    { Card c(17, "속독",         "다음 턴 드로우를 2장 추가합니다.");
      c.addEffect(draw(2)); reg.registerCard(c); }
    { Card c(18, "통찰",         "통찰을 얻어 드로우 1장 추가, HP 3 회복합니다.");
      c.addEffect(draw(1));
      c.addEffect(heal(3)); reg.registerCard(c); }
    { Card c(19, "아드레날린",    "각성해 드로우 2장 추가, 공격력을 2 높입니다.(1턴)");
      c.addEffect(draw(2));
      c.addEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(20, "독침",         " 3 데미지와 함께 독(1턴)을 부여합니다.");
      c.addEffect(attack(3));
      c.addEffect(poison(1)); reg.registerCard(c); }
    { Card c(21, "무장 해제",     "적에게 약화(1턴)를 부여해 데미지 출력을 25% 낮춥니다.");
      c.addEffect(weaken(1)); reg.registerCard(c); }
    { Card c(22, "약점 노출",     "약점을 노출시켜 적에게 취약(1턴)을 부여합니다.");
      c.addEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(23, "뇌진탕",       "강타로  4 데미지 후 마비(1턴)를 부여합니다.");
      c.addEffect(attack(4));
      c.addEffect(stun(1)); reg.registerCard(c); }
    { Card c(24, "약화의 일격",   " 5 데미지 후 데미지 출력을 25% 감소시킵니다.(1턴)");
      c.addEffect(attack(5));
      c.addEffect(weaken(1)); reg.registerCard(c); }
    { Card c(25, "방패 가격",     "방패로  6 데미지를 가하고 방어막 4를 생성합니다.(1턴)");
      c.addEffect(attack(6));
      c.addEffect(defense(4, 1)); reg.registerCard(c); }
    { Card c(26, "결집",         "파티 HP 4 회복, 파티 공격력을 1 높입니다.(1턴)");
      c.addEffect(heal(4, EffectTarget::Party));
      c.addEffect(Effect(EffectType::Buff, 1, 1, ST_ATK_UP, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(27, "일제 사격",     "전체 적에게  4 데미지를 퍼붓습니다.");
      c.addEffect(attack(4, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(28, "반격 태세",     "반격 자세로 방어막 10(1턴)과 공격력 +2(1턴)를 얻습니다.");
      c.addEffect(defense(10, 1));
      c.addEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(29, "이중 타격",     "빠르게  4 데미지를 두 번 가합니다.");
      c.addEffect(attack(4));
      c.addEffect(attack(4)); reg.registerCard(c); }
    { Card c(30, "최후의 저항",   "마지막 힘으로 HP 8 회복, 방어막 6을 생성합니다.(1턴)");
      c.addEffect(heal(8));
      c.addEffect(defense(6, 1)); reg.registerCard(c); }
    { Card c(32, "강제 교대",    "적이 아군의 자리를 강제로 바꾸는 혼란 카드. (적 전용)");
      c.addEffect(swap()); reg.registerCard(c); }

    // ── Nuclear (ID 101~120) ──────────────────────────────────────
    { Card c(101, "핵분열 폭발",     "핵분열 에너지로  8 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::Nuclear);
      c.addEffect(attack(8));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(102, "노심 용융",       "불안정한 연쇄반응으로  12 데미지와 독(3턴)을 부여합니다. [보너스] 약화 25%(1턴)", Track::Nuclear);
      c.addEffect(attack(12));
      c.addEffect(poison(3));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(103, "방사선 파동",     "방사선 파동으로  6 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::Nuclear);
      c.addEffect(attack(6));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(104, "격납 붕괴",       "방사능 누출로  7 데미지를 가합니다. [보너스] 전체 적 약화 25%(1턴)", Track::Nuclear);
      c.addEffect(attack(7));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(105, "방사선 조사",     "방사선을 주입해 독(3턴)을 부여합니다. [보너스] 독 1턴 추가", Track::Nuclear);
      c.addEffect(poison(3));
      c.addBonusEffect(poison(1)); reg.registerCard(c); }
    { Card c(106, "낙진",           "전체 적에게  5 데미지와 독(1턴)을 부여합니다. [보너스] 전체 독 1턴 추가", Track::Nuclear);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addEffect(poison(1, EffectTarget::AllEnemies));
      c.addBonusEffect(poison(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(107, "임계 질량",       "임계 도달로  15 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::Nuclear);
      c.addEffect(attack(15));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(108, "붕괴",           "장기 붕괴로 독(4턴)을 유발합니다. [보너스] 취약 50%(3턴)", Track::Nuclear);
      c.addEffect(poison(4));
      c.addBonusEffect(vulnerable(3)); reg.registerCard(c); }
    { Card c(109, "연쇄 반응",       "연쇄 반응으로  6 데미지와 독(1턴)을 부여합니다. [보너스]  4 데미지", Track::Nuclear);
      c.addEffect(attack(6));
      c.addEffect(poison(1));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(110, "중성자 창",       "중성자를 집속해  11 데미지를 가합니다. [보너스] 독(3턴) 부여", Track::Nuclear);
      c.addEffect(attack(11));
      c.addBonusEffect(poison(3)); reg.registerCard(c); }
    { Card c(111, "원자로 과부하",    "노심 과부하로  9+3 데미지를 가합니다. [보너스] 독(1턴) 부여", Track::Nuclear);
      c.addEffect(attack(9));
      c.addEffect(attack(3));
      c.addBonusEffect(poison(1)); reg.registerCard(c); }
    { Card c(112, "반감기",         "느리지만 끈질긴 붕괴로 독(5턴)을 부여합니다. [보너스] 약화 25%(1턴)", Track::Nuclear);
      c.addEffect(poison(5));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(113, "동위원소 탄",     "방사성 탄두로  7 데미지와 취약 50%(1턴)를 가합니다. [보너스] 독(1턴) 부여", Track::Nuclear);
      c.addEffect(attack(7));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(poison(1)); reg.registerCard(c); }
    { Card c(114, "원자 충격",       "원자 충격으로  13 데미지를 가합니다. [보너스] 약화 25%(1턴)", Track::Nuclear);
      c.addEffect(attack(13));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(115, "오염 지대",       "오염 지대를 생성해 전체 적에게 독(3턴)을 부여합니다. [보너스] 전체 취약 50%(1턴)", Track::Nuclear);
      c.addEffect(poison(3, EffectTarget::AllEnemies));
      c.addBonusEffect(vulnerable(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(116, "폐연료봉",        "폐연료봉을 투척해  6 데미지를 가합니다. [보너스] 독(4턴) 부여", Track::Nuclear);
      c.addEffect(attack(6));
      c.addBonusEffect(poison(4)); reg.registerCard(c); }
    { Card c(117, "감마선 폭발",     "감마선 폭발로  10 데미지를 가합니다. [보너스] 약화 25%(1턴)", Track::Nuclear);
      c.addEffect(attack(10));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(118, "다단 분열",       "다단 핵분열로  4+4 데미지와 독(1턴)을 부여합니다. [보너스] 독 1턴 추가", Track::Nuclear);
      c.addEffect(attack(4));
      c.addEffect(attack(4));
      c.addEffect(poison(1));
      c.addBonusEffect(poison(1)); reg.registerCard(c); }
    { Card c(119, "노심 직격",       "노심을 직격해  14 데미지와 취약 50%(1턴)를 가합니다. [보너스] 독(3턴) 부여", Track::Nuclear);
      c.addEffect(attack(14));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(poison(3)); reg.registerCard(c); }
    { Card c(120, "기폭",           "[시그니처] 전탄을 기폭해  18 데미지를 가합니다. [보너스]  6 데미지", Track::Nuclear);
      c.addEffect(attack(18));
      c.addBonusEffect(attack(6)); reg.registerCard(c); }

    // ── NewMaterial (ID 201~220) ──────────────────────────────────
    { Card c(201, "나노 블레이드",    "나노 날로  9 데미지를 가합니다. [보너스] 약화 25%(1턴)", Track::NewMaterial);
      c.addEffect(attack(9));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(202, "복합 방패",       "경화 소재로 파티 방어막 10을 부여합니다.(1턴) [보너스] 방어력 3 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(10, 1, EffectTarget::Party));
      c.addBonusEffect(defUp(3, 1)); reg.registerCard(c); }
    { Card c(203, "폴리머 타격",     "유연한 소재로  7 데미지와 HP 2를 회복합니다. [보너스]  4 데미지", Track::NewMaterial);
      c.addEffect(attack(7));
      c.addEffect(heal(2));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(204, "보호막 직조",     "파티 전체에 방어막 5를 부여합니다.(1턴) [보너스] 파티 방어력 2 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(5, 1, EffectTarget::Party));
      c.addBonusEffect(Effect(EffectType::Buff, 2, 1, ST_DEF_UP, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(205, "강화 장갑",       "강화 장갑으로 파티 방어막 12를 부여합니다.(1턴) [보너스] 파티 방어막 8 추가(1턴)", Track::NewMaterial);
      c.addEffect(defense(12, 1, EffectTarget::Party));
      c.addBonusEffect(defense(8, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(206, "반응 장갑",       "반응 장갑으로 파티 방어막 6을 부여하고 반격합니다.(1턴) [보너스]  5 데미지", Track::NewMaterial);
      c.addEffect(defense(6, 1, EffectTarget::Party));
      c.addBonusEffect(attack(5)); reg.registerCard(c); }
    { Card c(207, "탄소 격자",       "탄소 격자 강화로 방어력을 4 높입니다.(1턴) [보너스] 파티 방어막 5(1턴)", Track::NewMaterial);
      c.addEffect(defUp(4, 1));
      c.addBonusEffect(defense(5, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(208, "자가 복원",       "자가 복원으로 HP 8 회복, 파티 방어막 4를 부여합니다.(1턴) [보너스] HP 4 추가 회복", Track::NewMaterial);
      c.addEffect(heal(8));
      c.addEffect(defense(4, 1, EffectTarget::Party));
      c.addBonusEffect(heal(4)); reg.registerCard(c); }
    { Card c(209, "그래핀 칼날",     "그래핀 날로  10 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::NewMaterial);
      c.addEffect(attack(10));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(210, "요새",           "요새화로 파티 전체에 방어막 8을 부여합니다.(1턴) [보너스] 파티 방어력 2 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(8, 1, EffectTarget::Party));
      c.addBonusEffect(Effect(EffectType::Buff, 2, 1, ST_DEF_UP, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(211, "충격 흡수",       "충격 흡수 자세로 파티 방어막 10을 부여합니다.(1턴) [보너스] 파티 방어막 5 추가(1턴)", Track::NewMaterial);
      c.addEffect(defense(10, 1, EffectTarget::Party));
      c.addBonusEffect(defense(5, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(212, "형상기억 합금",    "형상기억 합금으로 HP 6 회복, 파티 방어막 6을 부여합니다.(1턴)", Track::NewMaterial);
      c.addEffect(heal(6));
      c.addEffect(defense(6, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(213, "인장 타격",       "인장 강도로  8 데미지와 파티 방어막 5를 부여합니다.(1턴) [보너스]  3 데미지", Track::NewMaterial);
      c.addEffect(attack(8));
      c.addEffect(defense(5, 1, EffectTarget::Party));
      c.addBonusEffect(attack(3)); reg.registerCard(c); }
    { Card c(214, "소모형 장갑",     "소모형 장갑으로 파티 방어막 15를 부여합니다.(1턴) [보너스] 방어력 3 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(15, 1, EffectTarget::Party));
      c.addBonusEffect(defUp(3, 1)); reg.registerCard(c); }
    { Card c(215, "나노직 보호",     "나노직 보호막으로 파티에 방어막 4를 부여합니다.(1턴) [보너스] 파티 HP 3 회복", Track::NewMaterial);
      c.addEffect(defense(4, 1, EffectTarget::Party));
      c.addBonusEffect(heal(3, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(216, "탄성 회복",       "탄성 회복으로 방어력을 5 높이고 HP 5를 회복합니다. [보너스] HP 5 추가 회복", Track::NewMaterial);
      c.addEffect(defUp(5, 1));
      c.addBonusEffect(heal(5)); reg.registerCard(c); }
    { Card c(217, "반격 행렬",       "반격 행렬로 파티 방어막 8(1턴)과  6 데미지를 가합니다. [보너스]  4 데미지", Track::NewMaterial);
      c.addEffect(defense(8, 1, EffectTarget::Party));
      c.addEffect(attack(6));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(218, "진영 보강",       "진영 보강으로 파티 방어막 7(1턴)과 방어력 2 증가(1턴)를 얻습니다. [보너스] 파티 방어막 5(1턴)", Track::NewMaterial);
      c.addEffect(defense(7, 1, EffectTarget::Party));
      c.addEffect(defUp(2, 1));
      c.addBonusEffect(defense(5, 1, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(219, "적응형 메시",     "적응형 메시로 파티 방어막 6(1턴)을 부여하고 드로우 1장을 추가합니다. [보너스] 방어력 2 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(6, 1, EffectTarget::Party));
      c.addEffect(draw(1));
      c.addBonusEffect(defUp(2, 1)); reg.registerCard(c); }
    { Card c(220, "불침투",         "[시그니처] 파티 방어막 10(1턴)과 공격 1회 차단을 부여합니다. [보너스] 파티 방어력 3 증가(1턴)", Track::NewMaterial);
      c.addEffect(defense(10, 1, EffectTarget::Party));
      c.addEffect(block(1));
      c.addBonusEffect(Effect(EffectType::Buff, 3, 1, ST_DEF_UP, EffectTarget::Party)); reg.registerCard(c); }

    // ── Hydrogen (ID 301~320) ─────────────────────────────────────
    { Card c(301, "연료전지 폭발",    "수소 폭발로  10 데미지를 가합니다. [보너스]  5 데미지", Track::Hydrogen);
      c.addEffect(attack(10));
      c.addBonusEffect(attack(5)); reg.registerCard(c); }
    { Card c(302, "극저온 분사",     "극저온 스트림으로  7 데미지와 마비(1턴)를 부여합니다. [보너스] 약화 25%(1턴)", Track::Hydrogen);
      c.addEffect(attack(7));
      c.addEffect(stun(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(303, "플라즈마 아크",    "고온 플라즈마로  11 데미지를 가합니다. [보너스]  4 데미지", Track::Hydrogen);
      c.addEffect(attack(11));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(304, "교란 가스",       "신경 교란 가스로  6 데미지를 가합니다. [보너스] 혼란(1턴) 부여", Track::Hydrogen);
      c.addEffect(attack(6));
      c.addBonusEffect(confuse(40, 1)); reg.registerCard(c); }
    { Card c(305, "순간 빙결",       "순간 빙결로 마비(1턴)를 부여합니다. [보너스]  6 데미지", Track::Hydrogen);
      c.addEffect(stun(1));
      c.addBonusEffect(attack(6)); reg.registerCard(c); }
    { Card c(306, "수소 폭탄",       "수소 폭탄으로  16 데미지를 가합니다. [보너스]  6 데미지", Track::Hydrogen);
      c.addEffect(attack(16));
      c.addBonusEffect(attack(6)); reg.registerCard(c); }
    { Card c(307, "동상",           "동상으로  8 데미지와 마비(1턴)를 부여합니다. [보너스] 약화 25%(1턴)", Track::Hydrogen);
      c.addEffect(attack(8));
      c.addEffect(stun(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(308, "압력 급등",       "압력 급상승으로  9 데미지를 가합니다. [보너스] 마비(1턴) 부여", Track::Hydrogen);
      c.addEffect(attack(9));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(309, "냉각재 분사",     "냉각재를 분사해 전체 적에게  5 데미지를 가합니다. [보너스] 전체 약화 25%(1턴)", Track::Hydrogen);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(310, "점화",           "점화 가속으로  7 데미지와 공격력 3 증가(1턴)를 얻습니다. [보너스]  4 데미지", Track::Hydrogen);
      c.addEffect(attack(7));
      c.addEffect(atkUp(3, 1));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(311, "초저온",         "초저온으로 마비(1턴)와 약화 25%(1턴)를 부여합니다. [보너스] 약화 25%(1턴) 추가", Track::Hydrogen);
      c.addEffect(stun(1));
      c.addEffect(weaken(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(312, "제트 기류",       "연속 분사로  6 데미지를 두 번 가합니다. [보너스]  4 데미지", Track::Hydrogen);
      c.addEffect(attack(6));
      c.addEffect(attack(6));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(313, "빙결 창",         "빙결 창으로  12 데미지를 가합니다. [보너스] 마비(1턴) 부여", Track::Hydrogen);
      c.addEffect(attack(12));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(314, "기화 베기",       "기화 베기로  8 데미지를 가합니다. [보너스] 회피 50%(1턴)", Track::Hydrogen);
      c.addEffect(attack(8));
      c.addBonusEffect(evade(1)); reg.registerCard(c); }
    { Card c(315, "심층 동결",       "심층 동결로 마비(3턴)를 부여합니다. [보너스] 약화 25%(1턴)", Track::Hydrogen);
      c.addEffect(stun(3));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(316, "연소",           "연소 폭발로  10 데미지와 공격력 2 증가(1턴)를 얻습니다. [보너스]  5 데미지", Track::Hydrogen);
      c.addEffect(attack(10));
      c.addEffect(atkUp(2, 1));
      c.addBonusEffect(attack(5)); reg.registerCard(c); }
    { Card c(317, "액체질소",       "액체질소를 분사해  7 데미지와 마비(1턴)를 부여합니다. [보너스] 약화 25%(1턴)", Track::Hydrogen);
      c.addEffect(attack(7));
      c.addEffect(stun(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(318, "과압",           "과압 파열로  13 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::Hydrogen);
      c.addEffect(attack(13));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(319, "서리 폭발",       "빙결 폭발로 전체 적에게  5 데미지와 마비(1턴)를 부여합니다. [보너스] 전체  3 데미지", Track::Hydrogen);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addEffect(stun(1, EffectTarget::AllEnemies));
      c.addBonusEffect(attack(3, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(320, "절대영도",       "[시그니처] 절대영도로  14 데미지와 마비(1턴)를 부여합니다. [보너스]  6 데미지", Track::Hydrogen);
      c.addEffect(attack(14));
      c.addEffect(stun(1));
      c.addBonusEffect(attack(6)); reg.registerCard(c); }

    // ── EcoTech (ID 401~420) ──────────────────────────────────────
    { Card c(401, "돌풍 베기",       "바람을 타고  8 데미지를 가합니다. [보너스] 공격력 2 증가(1턴)", Track::EcoTech);
      c.addEffect(attack(8));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(402, "진공 파열",       "급격한 기압 하강으로  6 데미지와 취약 50%(1턴)를 가합니다. [보너스] 마비(1턴) 부여", Track::EcoTech);
      c.addEffect(attack(6));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(403, "터빈 킥",         "터빈 회전력으로  9 데미지를 가합니다. [보너스] 공격력 3 증가(1턴)", Track::EcoTech);
      c.addEffect(attack(9));
      c.addBonusEffect(atkUp(3, 1)); reg.registerCard(c); }
    { Card c(404, "광합성",         "광합성 에너지로 HP 8을 회복합니다. [보너스] 파티 HP 4 회복", Track::EcoTech);
      c.addEffect(heal(8));
      c.addBonusEffect(heal(4, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(405, "순풍",           "순풍으로 파티 공격력을 2 높입니다.(1턴) [보너스] 드로우 1장 추가", Track::EcoTech);
      c.addEffect(Effect(EffectType::Buff, 2, 1, ST_ATK_UP, EffectTarget::Party));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(406, "태양 폭발",       "태양광을 집속해  7 데미지를 가합니다. [보너스] 약화 25%(1턴)", Track::EcoTech);
      c.addEffect(attack(7));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(407, "정화의 비",       "정화의 비로 파티 HP를 5 회복합니다. [보너스] 파티 HP 3 추가 회복", Track::EcoTech);
      c.addEffect(heal(5, EffectTarget::Party));
      c.addBonusEffect(heal(3, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(408, "상승 기류",       "상승 기류로 회피율을 50% 높입니다.(1턴) [보너스] 공격력 2 증가(1턴)", Track::EcoTech);
      c.addEffect(evade(1));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(409, "초목 성장",       "초목 성장으로 파티 HP 4 회복, 방어력 2 증가(1턴). [보너스] 파티 HP 3 추가 회복", Track::EcoTech);
      c.addEffect(heal(4, EffectTarget::Party));
      c.addEffect(Effect(EffectType::Buff, 2, 1, ST_DEF_UP, EffectTarget::Party));
      c.addBonusEffect(heal(3, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(410, "허리케인",       "태풍으로 전체 적에게  5 데미지를 가합니다. [보너스] 전체 약화 25%(1턴)", Track::EcoTech);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(411, "미풍 일격",       "미풍 일격으로  10 데미지를 가합니다. [보너스] 회피 50%(1턴)", Track::EcoTech);
      c.addEffect(attack(10));
      c.addBonusEffect(evade(1)); reg.registerCard(c); }
    { Card c(412, "지열",           "지열로 HP 6을 회복하고  5 데미지를 가합니다. [보너스] HP 4 추가 회복", Track::EcoTech);
      c.addEffect(heal(6));
      c.addEffect(attack(5));
      c.addBonusEffect(heal(4)); reg.registerCard(c); }
    { Card c(413, "수분",           "수분 확산으로 파티 공격력을 3 높입니다.(1턴) [보너스] 파티 HP 3 회복", Track::EcoTech);
      c.addEffect(Effect(EffectType::Buff, 3, 1, ST_ATK_UP, EffectTarget::Party));
      c.addBonusEffect(heal(3, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(414, "폭풍의 눈",       "폭풍의 눈에서 회피 50%(1턴)와 HP 4를 회복합니다. [보너스] 공격 1회 차단(1턴)", Track::EcoTech);
      c.addEffect(evade(1));
      c.addEffect(heal(4));
      c.addBonusEffect(block(1)); reg.registerCard(c); }
    { Card c(415, "윈드시어",       "윈드시어로  8 데미지와 약화 25%(1턴)를 가합니다. [보너스] 약화 25%(1턴) 추가", Track::EcoTech);
      c.addEffect(attack(8));
      c.addEffect(weaken(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(416, "재생",           "재생 능력으로 HP 12를 회복합니다. [보너스] 파티 HP 6 회복", Track::EcoTech);
      c.addEffect(heal(12));
      c.addBonusEffect(heal(6, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(417, "잔잔한 바람",     "잔잔한 바람으로 파티 HP 5 회복, 회피 50%(1턴)를 얻습니다. [보너스] 파티 HP 3 추가 회복", Track::EcoTech);
      c.addEffect(heal(5, EffectTarget::Party));
      c.addEffect(evade(1));
      c.addBonusEffect(heal(3, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(418, "토네이도",       "토네이도로 전체 적에게  6 데미지와 약화 25%(1턴)를 가합니다. [보너스] 전체 마비(1턴) 부여", Track::EcoTech);
      c.addEffect(attack(6, EffectTarget::AllEnemies));
      c.addEffect(weaken(1, EffectTarget::AllEnemies));
      c.addBonusEffect(stun(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(419, "생울타리",       "생울타리로 파티에 방어막 5(1턴)와 HP 4를 부여합니다. [보너스] 파티 방어력 2 증가(1턴)", Track::EcoTech);
      c.addEffect(defense(5, 1, EffectTarget::Party));
      c.addEffect(heal(4, EffectTarget::Party));
      c.addBonusEffect(Effect(EffectType::Buff, 2, 1, ST_DEF_UP, EffectTarget::Party)); reg.registerCard(c); }
    { Card c(420, "대지의 축복",     "[시그니처] 대지의 축복으로 파티 HP를 10 회복합니다. [보너스] 파티 공격력 2 증가(1턴)", Track::EcoTech);
      c.addEffect(heal(10, EffectTarget::Party));
      c.addBonusEffect(Effect(EffectType::Buff, 2, 1, ST_ATK_UP, EffectTarget::Party)); reg.registerCard(c); }

    // ── AI (ID 501~520) ───────────────────────────────────────────
    { Card c(501, "알고리즘 타격",    "계산된 알고리즘으로  8 데미지를 가합니다. [보너스] 공격력 2 증가(1턴)", Track::AI);
      c.addEffect(attack(8));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(502, "신경망 과부하",    "신경망 과부하로  10 데미지와 약화 25%(1턴)를 가합니다. [보너스] 마비(1턴) 부여", Track::AI);
      c.addEffect(attack(10));
      c.addEffect(weaken(1));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(503, "데이터 삭제",     "데이터를 지워  5 데미지와 취약 50%(1턴)를 가합니다. [보너스] HP 4 회복", Track::AI);
      c.addEffect(attack(5));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(heal(4)); reg.registerCard(c); }
    { Card c(504, "재컴파일",       "시스템을 재컴파일해 드로우 2장을 추가합니다. [보너스] 드로우 1장 추가", Track::AI);
      c.addEffect(draw(2));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(505, "최적화",         "시스템을 최적화해 공격력을 3 높입니다.(1턴) [보너스] 드로우 1장 추가", Track::AI);
      c.addEffect(atkUp(3, 1));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(506, "병렬 처리",       "병렬 처리로 드로우 2장과  4 데미지를 가합니다. [보너스] 드로우 1장 추가", Track::AI);
      c.addEffect(draw(2));
      c.addEffect(attack(4));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(507, "오버클럭",       "오버클럭으로 공격력을 4 높입니다.(1턴) [보너스]  4 데미지", Track::AI);
      c.addEffect(atkUp(4, 1));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(508, "예측 조준",       "예측 조준으로  9 데미지를 가합니다. [보너스]  4 데미지", Track::AI);
      c.addEffect(attack(9));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(509, "캐시 적중",       "캐시 적중으로 드로우 1장과 HP 3을 회복합니다. [보너스] 드로우 1장 추가", Track::AI);
      c.addEffect(draw(1));
      c.addEffect(heal(3));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(510, "논리 폭탄",       "논리 폭탄으로  6 데미지와 독(1턴)을 부여합니다. [보너스] 취약 50%(1턴)", Track::AI);
      c.addEffect(attack(6));
      c.addEffect(poison(1));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(511, "심층 학습",       "심층 학습으로 드로우 3장을 추가합니다. [보너스] 공격력 2 증가(1턴)", Track::AI);
      c.addEffect(draw(3));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(512, "조준 행렬",       "조준 행렬로  7 데미지와 공격력 2 증가(1턴)를 얻습니다. [보너스]  3 데미지", Track::AI);
      c.addEffect(attack(7));
      c.addEffect(atkUp(2, 1));
      c.addBonusEffect(attack(3)); reg.registerCard(c); }
    { Card c(513, "무차별 대입",     "무차별 대입으로  12 데미지를 가합니다. [보너스]  4 데미지", Track::AI);
      c.addEffect(attack(12));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(514, "조각 모음",       "조각 모음으로 HP 6을 회복하고 드로우 1장을 추가합니다. [보너스] HP 3 추가 회복", Track::AI);
      c.addEffect(heal(6));
      c.addEffect(draw(1));
      c.addBonusEffect(heal(3)); reg.registerCard(c); }
    { Card c(515, "암호화",         "암호화로 공격 1회 차단(1턴)과 드로우 1장을 얻습니다. [보너스] 방어력 2 증가(1턴)", Track::AI);
      c.addEffect(block(1));
      c.addEffect(draw(1));
      c.addBonusEffect(defUp(2, 1)); reg.registerCard(c); }
    { Card c(516, "신경망",         "신경망으로 전체 적에게  5 데미지를 가합니다. [보너스] 공격력 2 증가(1턴)", Track::AI);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(517, "긴급 패치",       "긴급 패치로 HP를 8 회복합니다. [보너스] 드로우 1장 추가", Track::AI);
      c.addEffect(heal(8));
      c.addBonusEffect(draw(1)); reg.registerCard(c); }
    { Card c(518, "취약점 공략",     "취약점을 공략해  8 데미지와 취약 50%(1턴)를 가합니다. [보너스]  4 데미지", Track::AI);
      c.addEffect(attack(8));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(519, "재귀 타격",       "재귀 타격으로  4 데미지를 두 번 가하고 드로우 1장을 추가합니다. [보너스]  3 데미지", Track::AI);
      c.addEffect(attack(4));
      c.addEffect(attack(4));
      c.addEffect(draw(1));
      c.addBonusEffect(attack(3)); reg.registerCard(c); }
    { Card c(521, "스택 오버플로",    "손패 수만큼  3 데미지를 가합니다.", Track::AI);
      c.addEffect(handScaleAttack(3)); reg.registerCard(c); }
    { Card c(520, "특이점",         "[시그니처] 특이점 폭발로  10 데미지와 드로우 2장을 얻습니다. [보너스] 손패 수만큼  2 데미지 추가", Track::AI);
      c.addEffect(attack(10));
      c.addEffect(draw(2));
      c.addBonusEffect(handScaleAttack(2)); reg.registerCard(c); }

    // ── Grid (ID 601~620) ─────────────────────────────────────────
    { Card c(601, "전력 과부하",     "그리드를 과부하해  11 데미지를 가합니다. [보너스] 전체 적에게  5 데미지", Track::Grid);
      c.addEffect(attack(11));
      c.addBonusEffect(attack(5, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(602, "정전 타격",       "전력을 차단해  7 데미지와 마비(1턴)를 부여합니다. [보너스] 약화 25%(1턴)", Track::Grid);
      c.addEffect(attack(7));
      c.addEffect(stun(1));
      c.addBonusEffect(weaken(1)); reg.registerCard(c); }
    { Card c(603, "아크 방전",       "전기 방전으로  9 데미지를 가합니다. [보너스] 마비(1턴) 부여", Track::Grid);
      c.addEffect(attack(9));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(604, "연쇄 번개",       "연쇄 번개로 전체 적에게  6 데미지를 가합니다. [보너스] 전체  3 데미지", Track::Grid);
      c.addEffect(attack(6, EffectTarget::AllEnemies));
      c.addBonusEffect(attack(3, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(605, "과전압",         "과전압으로  13 데미지를 가합니다. [보너스] 취약 50%(1턴)", Track::Grid);
      c.addEffect(attack(13));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(606, "정전기장",       "정전기장으로 전체 적을 마비(1턴)시킵니다. [보너스] 전체 약화 25%(1턴)", Track::Grid);
      c.addEffect(stun(1, EffectTarget::AllEnemies));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(607, "뇌격",           "뇌격으로 전체 적에게  8 데미지를 가합니다. [보너스] 전체 마비(1턴) 부여", Track::Grid);
      c.addEffect(attack(8, EffectTarget::AllEnemies));
      c.addBonusEffect(stun(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(608, "전도체",         "전도체를 통해  7 데미지와 공격력 3 증가(1턴)를 얻습니다. [보너스]  4 데미지", Track::Grid);
      c.addEffect(attack(7));
      c.addEffect(atkUp(3, 1));
      c.addBonusEffect(attack(4)); reg.registerCard(c); }
    { Card c(609, "서지 방호",       "서지 방호로 방어막 8을 생성합니다.(1턴) [보너스] 공격력 2 증가(1턴)", Track::Grid);
      c.addEffect(defense(8, 1));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(610, "전자기 펄스",     "전자기 펄스로 전체 적을 마비(1턴)시킵니다. [보너스] 전체 약화 25%(1턴)", Track::Grid);
      c.addEffect(stun(1, EffectTarget::AllEnemies));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(611, "테슬라 코일",     "테슬라 코일로 전체  5, 단일  5 데미지를 가합니다. [보너스] 전체  3 데미지", Track::Grid);
      c.addEffect(attack(5, EffectTarget::AllEnemies));
      c.addEffect(attack(5));
      c.addBonusEffect(attack(3, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(612, "전압 급등",       "전압 급등으로  10 데미지를 가합니다. [보너스] 마비(1턴) 부여", Track::Grid);
      c.addEffect(attack(10));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(613, "계통 마비",       "계통 마비로 적을 마비(3턴)시킵니다. [보너스] 취약 50%(1턴)", Track::Grid);
      c.addEffect(stun(3));
      c.addBonusEffect(vulnerable(1)); reg.registerCard(c); }
    { Card c(614, "방전",           "전기를 방전해  14 데미지를 가합니다. [보너스] 전체 적에게  5 데미지", Track::Grid);
      c.addEffect(attack(14));
      c.addBonusEffect(attack(5, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(615, "축전기",         "축전기로 공격력을 4 높입니다.(1턴) [보너스] 방어막 5(1턴)", Track::Grid);
      c.addEffect(atkUp(4, 1));
      c.addBonusEffect(defense(5, 1)); reg.registerCard(c); }
    { Card c(616, "구상 번개",       "구전으로 전체 적에게  7 데미지를 가합니다. [보너스] 전체 약화 25%(1턴)", Track::Grid);
      c.addEffect(attack(7, EffectTarget::AllEnemies));
      c.addBonusEffect(weaken(1, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(617, "전기 충격",       "전기 충격으로  6 데미지와 마비(1턴)를 부여합니다. [보너스] 공격력 2 증가(1턴)", Track::Grid);
      c.addEffect(attack(6));
      c.addEffect(stun(1));
      c.addBonusEffect(atkUp(2, 1)); reg.registerCard(c); }
    { Card c(618, "전력망",         "전력망 분배로 파티 공격력을 2 높입니다.(1턴) [보너스] 전체  5 데미지", Track::Grid);
      c.addEffect(Effect(EffectType::Buff, 2, 1, ST_ATK_UP, EffectTarget::Party));
      c.addBonusEffect(attack(5, EffectTarget::AllEnemies)); reg.registerCard(c); }
    { Card c(619, "피뢰침",         "피뢰침으로  9 데미지와 취약 50%(1턴)를 가합니다. [보너스] 마비(1턴) 부여", Track::Grid);
      c.addEffect(attack(9));
      c.addEffect(vulnerable(1));
      c.addBonusEffect(stun(1)); reg.registerCard(c); }
    { Card c(620, "폭풍 해일",       "[시그니처] 폭풍 해일로 전체 적에게  8 데미지와 마비(1턴)를 부여합니다. [보너스] 전체  4 데미지", Track::Grid);
      c.addEffect(attack(8, EffectTarget::AllEnemies));
      c.addEffect(stun(1, EffectTarget::AllEnemies));
      c.addBonusEffect(attack(4, EffectTarget::AllEnemies)); reg.registerCard(c); }
}
