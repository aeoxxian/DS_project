# 텍스트 기반 카드 배틀 게임 기획서

## 개요

Slay the Spire 스타일의 텍스트 기반 카드 배틀 게임.
C++ 자료구조 PBL 프로젝트.

---

## 맵 구조

트리형 맵. 어느 경로를 택해도 방 10개.

```
[시작]
  ├─ [전투] ─── [이벤트] ─┬─ [전투] ──┬─ [보스]
  │                       └─ [휴식]  ─┘
  └─ [이벤트] ─ [전투] ───┬─ [휴식]  ─┤
                          └─ [전투] ──┘
```

- 플레이어는 매 분기에서 **한 갈래만 선택**해 전진
- 모든 경로는 최종 보스 방으로 수렴

### 방 종류

| 종류 | 내용 |
|------|------|
| 전투 (Battle) | 적 1~2마리와 카드 배틀. 승리 시 카드 보상 |
| 이벤트 (Event) | 텍스트 선택지. 이득/손해/중립 결과 |
| 휴식 (Rest) | HP 회복 또는 카드 강화 선택 |
| 보스 (Boss) | 강력한 단일 적. 클리어 시 런 종료 |

---

## 트랙 시스템

총 6개의 트랙. 캐릭터 1명당 **트랙 1개 고정**.

| 트랙 | 키 | 플레이 컨셉 | 시그니처 메커니즘 |
|------|----|------------|----------------|
| 핵 | Nuclear | **세미 탱커 + 독·디버프 소모전** — 어느 정도 버티면서 독과 디버프로 적을 서서히 갉아먹음 | poison 스택, weaken, defDown, 준수한 DEF |
| 신소재 | NewMaterial | **방어 탱커** — 방어막과 방어력으로 버티며 반격 | defense, defUp, block |
| 수소 | Hydrogen | **동결+폭발 딜러** — 극저온으로 적을 얼리거나 폭발적 순간 고딜. 디버프 없이 CC와 데미지만 | stun, 순간 고딜 |
| 환경기후기술 | EcoTech | **파티 서포터** — 회복과 회피로 파티를 지탱하며 버프 제공 | heal, evade, party 버프 |
| AI | AI | **드로우 엔진 + 세미 서포터** — 드로우로 파티 자원을 늘리고 해킹으로 적을 무력화 | draw, stun·confuse·atkDown·defDown |
| 그리드 | Grid | **광역 딜러** — 단일 전기 공격이 트랙 일치 시 전체 적으로 확산 | 보너스: magicAttack AllEnemies, 광역 stun |

### 트랙별 운영 방식

**Nuclear** — 준수한 DEF로 버티면서 독을 쌓고 weaken·defDown으로 적의 저항력을 낮추는 소모전. poison은 스택이 쌓일수록 매 턴 데미지와 지속 턴이 같이 증가. 순간 폭딜은 없지만 장기전에서 강함.

**NewMaterial** — 방어막(defense)으로 피해를 막고 defUp으로 방어력을 올리며 장기전. block으로 치명타 차단.

**Hydrogen** — 극저온 카드(Cryo, Freeze, Zero Point 계열)로 적을 stun시키거나, 수소 폭발 카드(Hydrogen Bomb, Fuel Cell Blast 계열)로 순간 고딜. 디버프 없이 CC와 데미지 두 가지만 운용.

**EcoTech** — 파티 heal과 evade로 생존력을 올리면서 atkUp 버프로 딜을 보조. 단독으로 싸우기보단 파티 전체를 강하게 만드는 역할.

**AI** — draw 카드로 손패를 채워 파티 자원을 지원. 해킹 컨셉으로 적을 무력화(stun·confuse·atkDown·defDown)하는 디버프도 겸비한 세미 서포터.

**Grid** — 기본 이펙트는 단일 마법 공격이지만, 트랙 일치 시 보너스로 전체 적에게 추가 마법 데미지 또는 전체 마비. 적이 많을수록 강함.

---

## 카드 시스템

### 카드 종류

| 종류 | 기본 이펙트 | 보너스 이펙트 |
|------|------------|--------------|
| 공용 카드 (Track::None) | 항상 발동 | 없음 |
| 트랙 카드 | 항상 발동 | 트랙 일치 시 추가 발동 |

### 카드풀 구성
- 공용 카드 + 파티 트랙에 해당하는 트랙 카드로 구성
- 파티에 없는 트랙의 카드는 풀에 포함되지 않음
- **구현: LinkedList 기반 CardPool**

### 이펙트 종류

| 타입 | 설명 |
|------|------|
| PhysAttack | 물리 데미지 (ATK 스케일, DEF 감소) |
| MagicAttack | 마법 데미지 (MGC 스케일, DEF 감소) |
| HandScaleAttack | 현재 손패 수만큼 마법 공격 반복 (MGC 스케일, DEF 감소) |
| Defense | 방어막 생성 |
| Buff | 아군 강화 / 회복 |
| Debuff | 적 상태이상 |
| Draw | 다음 턴 추가 드로우 |

### 덱 구조
- 모든 캐릭터가 **공유하는 카드풀 (LinkedList)**
- 손패도 **LinkedList 기반 Hand**
- 매 턴 뽑은 카드는 사용 여부 무관하게 **턴 종료 시 전부 복귀**

---

## 전투 시스템

| 항목 | 내용 |
|------|------|
| 아군 캐릭터 | 3명 (런 시작 시 로스터에서 선택) |
| 손패 최대 | 10장 |
| 매턴 드로우 | 5장 |

### 턴 흐름
1. 카드풀에서 5장 드로우 → 손패(LinkedList)에 추가
2. 손패에서 캐릭터 1명당 1장씩 분배
3. 각 캐릭터 행동 (트랙 일치 시 보너스 발동)
4. 상태이상 틱 (독 데미지, 턴 차감)
5. 적 행동
6. 턴 종료 → 손패 전부 풀 복귀

### 스탯

| 스탯 | 설명 |
|------|------|
| ATK | 물리 공격력. PhysAttack 카드 데미지에 더해짐 |
| DEF | 방어력. 물리·마법 공격 모두 감소. 버프/디버프로 증감 가능 |
| MGC | 주문력. MagicAttack·HandScaleAttack 카드 데미지에 더해짐 |

```
getAttackPower() = baseATK + Σatk_up - Σatk_down
getDefense()     = baseDEF + Σdef_up - Σdef_down
getMagicPower()  = baseMGC + Σmag_up - Σmag_down
```

### 데미지 계산

**물리 공격 (PhysAttack)**
```
출력 = 카드 value + 공격자 ATK
출력 *= (100 - weaken%) / 100     (weaken 있을 때)
출력 *= 150 / 100                  (수비자 vulnerable 있을 때)
실제 데미지 = 출력 - 수비자 DEF   (최소 0)
```

**마법 공격 (MagicAttack)**
```
출력 = 카드 value + 공격자 MGC
출력 *= (100 - weaken%) / 100     (weaken 있을 때)
출력 *= 150 / 100                  (수비자 vulnerable 있을 때)
실제 데미지 = 출력 - 수비자 DEF   (최소 0)
```

**손패 연타 (HandScaleAttack)**
```
1회 출력 = 카드 value + 공격자 MGC
1회 출력 *= (100 - weaken%) / 100  (weaken 있을 때)
1회 실제 데미지 = 1회 출력 - 수비자 DEF  (최소 0)
총 공격 횟수 = 현재 손패 수
```

**회복 (heal)**
```
회복량 = 카드 value  (스탯 스케일 없음)
회복량 *= (100 - burn%) / 100      (대상에게 burn 있을 때)
HP 최대치 초과 불가
```

### 상태이상 틱 순서 (매 턴 캐릭터/적 모두)
1. `stun` — 행동 불가 판정
2. `confuse` — 30% 확률로 본인 공격
3. `poison` — value만큼 HP 감소
4. `burn` — 회복 시 회복량 감소 (틱 데미지 아님)
5. 모든 상태이상 duration 1 감소 → 0 되면 제거

### 적 의도 시스템 (Queue)
- 적은 `Queue<EnemySkill>`로 행동 패턴을 관리
- 행동 패턴은 `addSkill()` 순서대로 순환 (1→2→3→1→...)
- 플레이어에게는 **다음 행동 1개만** peek으로 공개
- 행동 실행 후 다음 패턴 행동 자동 enqueue

### 전투 후 보상
- 카드 3장 제시 → 각각 **추가 / 교체 / 스킵** 선택

### 런 종료 통계
- 모든 전투의 입힌 데미지 / 받은 데미지 / 턴 수 기록
- **Sorting**: 입힌 데미지 기준 내림차순 정렬 출력
- **BST (ScoreTree)**: 효율 점수(`damageDealt - damageTaken`) 기준 랭킹 출력

---

## 상태이상

### 스태킹 규칙

| 종류 | 동작 |
|------|------|
| `poison` | **합산** — value·duration 모두 누적. 스택을 쌓을수록 매 턴 데미지와 지속 턴이 같이 증가 |
| 나머지 전부 | **독립 슬롯** — 각각 따로 틱다운. `getModifier()`는 같은 키의 value를 전부 합산 |

### Debuff (적에게 적용)

| 키 | 효과 | 스태킹 |
|----|------|--------|
| poison | 매 턴 value만큼 HP 감소 | value+duration 합산 |
| weaken | 공격자의 출력 감소 (%) | value 합산 |
| atk_down | 공격력 감소 | value 합산 |
| def_down | 방어력 감소 | value 합산 |
| burn | 치유량 감소 (%) | value 합산 |
| confuse | 30% 확률로 본인 공격 | 독립 슬롯 |
| stun | 행동 불가 | 독립 슬롯 |
| vulnerable | 받는 출력 50% 증가 | 독립 슬롯 |

### Buff (아군에게 적용)

| 키 | 효과 |
|----|------|
| heal | HP 회복 (card value 고정, 스탯 스케일 없음) |
| atk_up | 공격력 증가 |
| def_up | 방어력 증가 (물리·마법 모두 적용) |
| mag_up | 주문력 증가 |
| evade | N% 확률로 공격 회피 |
| block | 다음 공격 1회 완전 차단 |
| shield | 방어막 (Defense 타입으로 처리) |

---

## 자료구조 매핑

| 자료구조 | 구현 위치 | 게임 내 역할 |
|----------|-----------|-------------|
| **LinkedList** | `include/ds/LinkedList.h` | CardPool (공유 덱), Hand (손패) |
| **Stack** | `include/ds/Stack.h` | DungeonGraph DFS 탐색 (`dfs()`, `printMap()` 내부) |
| **Queue** | `include/ds/Queue.h` | 적 의도 시스템 (`Enemy::intentQueue`) |
| **BST** | `include/ds/ScoreTree.h` | 전투 효율 랭킹 — `damageDealt - damageTaken` 기준 내림차순 출력 |
| **Graph** | `include/map/DungeonGraph.h` | 던전 맵 — 방(Room) 간 방향 연결 및 탐색 |
| **Sorting** | `include/ds/Sorting.h` | 런 종료 전투 통계 정렬 |
| **DynamicArray** | `include/ds/DynamicArray.h` | Room 내 적/아이템 목록 |

---

## 미정 사항

- [ ] 캐릭터 로스터 설계 (각 트랙별 캐릭터 스탯·이름)
- [ ] 이벤트 내용 및 선택지
- [ ] 보스 설계
