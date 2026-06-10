# Dungeon Explorer / 지식의 상아탑 — 게임 기획서

Slay the Spire 스타일의 C++17 텍스트 기반 카드 배틀 로그라이크.  
C++ 자료구조 PBL 과제 — STL 컨테이너 사용 금지, 모든 DS 직접 구현.

---

## 맵 구조

3층 × 3행 × 3열 = 27개 방. `RunMap::LAYOUT[3][3][3]`에 고정 레이아웃 정의.

```
[Start]
  └─ w/a/s/d 이동 → DungeonGraph::getNeighbor(현재방, 방향)

층 1: Start → Battle / Event 분기
층 2: Battle / Rest / Event 조합
층 3: Boss
```

- 모든 이동은 `DungeonGraph`의 4방향 인접 배열을 통해 처리
- `RunMap::history` (Stack) 에 이동 이력 저장 → `u/undo`로 복귀
- 방문·클리어 상태는 `Room::cleared` 플래그로 관리 (재진입 시 효과 미재발생)
- `g/graph` 명령으로 DFS 던전 구조 출력 가능

### 방 종류

| 종류 | 내용 |
|------|------|
| Start | 시작 지점, 효과 없음 |
| Battle | 일반 몬스터 1~2마리. 승리 시 카드 보상 (추가/교체/스킵) |
| Event | 이벤트 6종 중 무작위 1개 발동 |
| Rest | HP +20 회복 / 영구 공격력 +1 / 영구 방어력 +1 중 선택 |
| Stairs | 다음 층으로 진행 |
| Boss | 보스 1마리. 클리어 시 런 종료 |

---

## 트랙 시스템

캐릭터 1명당 트랙 1개 고정. 트랙 카드 사용 시 보너스 이펙트 추가 발동.

| 트랙 | 코드명 | 플레이 컨셉 | 핵심 메커니즘 |
|------|--------|------------|--------------|
| 핵 | Nuclear | 세미 탱커 + 독·디버프 소모전 | poison 스택, weaken, defDown |
| 신소재 | NewMaterial | 방어 탱커 — 파티 방어막으로 버팀 | defense(Party), defUp, block |
| 수소 | Hydrogen | 동결+폭발 딜러 | stun, 순간 고딜 |
| 환경기후기술 | EcoTech | 파티 서포터 | heal, evade, party 버프 |
| AI | AI | 드로우 엔진 + 세미 서포터 | draw, stun·confuse·atkDown·defDown |
| 그리드 | Grid | 광역 딜러 | 보너스: attack(AllEnemies), 광역 stun |

**트랙별 운영 요약**

- **Nuclear** — 준수한 DEF로 버티면서 독을 쌓고 weaken/defDown으로 적의 저항을 낮추는 장기전. poison은 value·duration 합산 스택.
- **NewMaterial** — 방어막과 defUp으로 장기 생존. 방어 카드가 파티 전체에 방어막을 씌워 팀 생존력을 높임.
- **Hydrogen** — 극저온 카드(stun)와 수소 폭발(순간 고딜) 두 가지 운용. 디버프 없이 CC와 데미지만.
- **EcoTech** — 파티 heal·evade로 생존력 확보, atkUp 버프로 파티 딜 보조. 단독 전투력보다 팀 강화에 특화.
- **AI** — draw 카드로 파티 자원 지원, 해킹 디버프(stun·confuse·atkDown·defDown)로 적 무력화.
- **Grid** — 기본 단일 마법 공격이 트랙 일치 시 전체 적으로 확산. 다수 적 전투에서 최강.

---

## 카드 시스템

### 카드 구성 (총 152장)

| 종류 | 수량 | 비고 |
|------|------|------|
| 공용 카드 (Track::None) | 31장 | 어느 캐릭터나 사용 가능 |
| 트랙 카드 × 6 | 각 20~21장 | 트랙 일치 시 보너스 이펙트 발동 |

### 덱 빌딩 (런 시작 시)

1. 캐릭터 3명 선택 → 2. 덱 빌딩 화면
   - **공용 카드 10장** 선택
   - **캐릭터 1 트랙 카드 5장** 선택
   - **캐릭터 2 트랙 카드 5장** 선택
   - **캐릭터 3 트랙 카드 5장** 선택
   - 총 **25장** 고정 구성
   - 각 단계에서 `[u]`로 마지막 선택 Undo 가능

### 이펙트 종류

| 타입 | 설명 |
|------|------|
| Attack | 물리 데미지 (ATK 스케일, DEF 감소) |
| HandScaleAttack | 현재 손패 수만큼 Attack 반복 |
| Defense | 방어막 생성 (단일 or 파티 전체) |
| Heal | HP 회복 (단일 or 파티 전체) |
| Buff | atkUp / defUp / evade / block |
| Debuff | atkDown / defDown / weaken / poison / stun / confuse / burn / vulnerable |
| Draw | 다음 턴 드로우 +v |
| Swap | 강제 파티 포지션 랜덤 교체 (적 전용) |

### 덱 구조

- 모든 캐릭터가 공유하는 **LinkedList 기반 CardPool**
- 손패(`Hand`)는 **고정 슬롯 배열 + `bool active[]`** — 카드 사용 시 슬롯 비활성화(인덱스 고정), Undo 시 원래 슬롯 복귀
- 매 턴 뽑은 카드는 사용 여부 무관하게 **턴 종료 시 전부 CardPool로 복귀**

---

## 전투 시스템

| 항목 | 값 |
|------|----|
| 파티 크기 | 3명 고정 |
| 손패 최대 | 10장 |
| 매턴 드로우 | 5장 |
| 적 수 | 일반 1~2마리, 보스 1마리 |

### 턴 흐름

```
1. drawPhase   — CardPool에서 5장 드로우 → Hand 슬롯에 배치
2. assignPhase — 캐릭터 3명에 카드 1장씩 배정 (Stack에 기록 → Undo 가능)
3. executePhase — 플레이어 카드 실행 → 상태이상 틱 → 적 카드 실행
4. rewardPhase — (전투 승리 시) 카드 3장 제시 → 추가/교체/스킵
5. 턴 종료    — Hand 전부 CardPool로 복귀
```

### 전열 시스템

- 파티 인덱스 0부터 첫 번째 생존자가 **전열(★)** — 적의 단일 공격은 전열만 받음
- 배정 단계에서 `switch` 입력 → 두 캐릭터 자리 바꾸기 (행동 소비)
- 적의 Swap 카드 → 파티 포지션 랜덤 교체

### 데미지 계산

```
공격 출력 = card.value + 공격자 ATK
         × (100 - weaken%) / 100   [weaken 있을 때]
         × 150 / 100               [수비자 vulnerable 있을 때]
실제 데미지 = max(0, 출력 - 수비자 DEF)
```

HandScaleAttack: 위 계산을 현재 손패 수만큼 반복.  
회복: `card.value` 고정 (ATK 스케일 없음), burn 시 감소.

### 상태이상 틱 순서 (매 턴, 캐릭터·적 공통)

1. `stun` — 행동 불가 판정
2. `confuse` — 30% 확률로 본인 공격
3. `poison` — value만큼 HP 감소
4. `burn` — 회복량 감소 (틱 데미지 아님)
5. 전체 duration 1 감소 → 0이면 제거

### 적 의도 시스템 (Queue)

- `Enemy::intentQueue` (`Queue<Card>`) 에 다음 행동 1개 유지
- `prepareIntent()` → enqueue / `executeAndQueue()` → dequeue 후 다음 패턴 enqueue (무한 순환)
- 전투 화면에 **카드 이름 + 설명** 미리 공개 (`peekIntent`)
- stun 시 행동은 건너뛰지만 패턴 인덱스는 진행

### 영구 스탯 부스트 (Rest)

`Combatant::permBoostAtk(1)` / `permBoostDef(1)` — `attackPower` / `defend` 직접 증가, 해당 런 내내 유지.

---

## 상태이상 상세

### 스태킹 규칙

| 상태이상 | 스태킹 |
|----------|--------|
| `poison` | value·duration **합산** — 스택이 쌓일수록 틱 데미지·지속 턴 동시 증가 |
| 나머지 | **독립 슬롯** — `getModifier()`는 같은 키의 value 전부 합산 |

### Debuff

| 키 | 효과 |
|----|------|
| poison | 매 턴 value HP 감소 |
| weaken | 공격자 출력 감소 (%) |
| atk_down | 공격력 감소 |
| def_down | 방어력 감소 |
| burn | 회복량 감소 (%) |
| confuse | 30% 자해 |
| stun | 행동 불가 |
| vulnerable | 받는 출력 50% 증가 |

### Buff

| 키 | 효과 |
|----|------|
| atk_up | 공격력 증가 |
| def_up | 방어력 증가 |
| evade | N% 공격 회피 |
| block | 다음 공격 1회 완전 차단 |

---

## 이벤트 시스템

이벤트 방 진입 시 등록된 이벤트 중 무작위 1개 발동. 클리어된 방 재진입 시 재발생 없음.

### 이벤트 목록

| ID | 이름 | 종류 | 효과 |
|----|------|------|------|
| 1 | 갑작스러운 정전 | 강제 | 파티 전체 피해 5 |
| 2 | 대학원생의 연구노트 | 선택 | ①카드 1장 / ②카드 1장 + 피해 4 |
| 3 | 교수님의 노트북 | 강제 | 덱에서 카드 1장 선택 제거 |
| 4 | 도서관 | 선택 | ①카드 1장 / ②HP +10 / ③카드 1장 + 피해 5 |
| 5 | 수상한 용액 | 선택 | ①파티 피해 8 / ②파티 HP +15 |
| 6 | 수상한 상점 | 강제 | 무료 보급 — 카드 또는 포션 1개 무료 선택 |

### OutcomeType

| 타입 | 효과 |
|------|------|
| `HealParty` | 파티 전체 HP 회복 |
| `DamageParty` | 파티 전체 피해 |
| `AddCard` | 무작위 카드 1장 획득 |
| `RemoveSelectedCard` | 플레이어가 선택한 카드 제거 |
| `OpenShop` | 무료 보급 오픈 (카드 or 포션 1개 무료 선택) |
| `GainGold` | 미사용 (골드 시스템 제거됨) |
| `LoseGold` | 미사용 (골드 시스템 제거됨) |
| `Nothing` | 효과 없음 |

---

## 포션 & 인벤토리

| 포션 | 효과 |
|------|------|
| 소형 회복 포션 | HP +15 |
| 중형 회복 포션 | HP +30 |
| 대형 회복 포션 | HP +50 |
| 공격력 포션 | ATK +8 (3턴) |
| 방어막 포션 | Shield +20 (2턴) |
| 독 포션 | 적에게 poison 8/t, 3턴 |
| 폭발 포션 | 전체 적 -25 HP |

- 인벤토리: `LinkedList<Item>` — 최대 `MAX_POTIONS`개
- 전투 중 `use` 명령으로 사용
- 무료 보급(이벤트 6번) 또는 이벤트를 통해 획득

---

## 자료구조 매핑

| 자료구조 | 구현 | 게임 내 역할 |
|----------|------|-------------|
| LinkedList | `ds/LinkedList.h` | CardPool(공유 덱), Inventory(포션) |
| Stack | `ds/Stack.h` | RunMap 이동 Undo, Battle 카드 배정 Undo, DungeonGraph DFS |
| Queue | `ds/Queue.h` | Enemy::intentQueue — 행동 패턴 순환 |
| DynamicArray | `ds/DynamicArray.h` | Room 내 아이템·적 목록 |
| BST (ScoreTree) | `ds/ScoreTree.h` + `.cpp` | 전투 효율 점수 랭킹 |
| Graph (DungeonGraph) | `map/DungeonGraph.h` + `.cpp` | 4방향 방 연결 + Stack DFS |
| Sorting | `ds/Sorting.h` + `.cpp` | 인벤토리·전투 통계·점수 순위 정렬 |

---

## 몬스터 스탯 요약

### 일반 몬스터 (HP 원본 대비 ~30% 감소, ATK·DEF 원본 유지)

| 이름 | 트랙 | HP | ATK | DEF |
|------|------|----|-----|-----|
| Fission Hound | Nuclear | 40 | 8 | 4 |
| Reactor Wraith | Nuclear | 65 | 12 | 6 |
| Nano Swarm | NewMaterial | 35 | 7 | 5 |
| Carbon Golem | NewMaterial | 80 | 10 | 14 |
| Cryo Crawler | Hydrogen | 40 | 9 | 5 |
| Plasma Leech | Hydrogen | 55 | 11 | 6 |
| Gust Wraith | EcoTech | 30 | 8 | 4 |
| Turbine Beast | EcoTech | 60 | 13 | 8 |
| Rogue Drone | AI | 40 | 9 | 5 |
| Neural Phantom | AI | 65 | 11 | 6 |
| Static Fiend | Grid | 45 | 8 | 6 |
| Overload Titan | Grid | 90 | 14 | 10 |

### 보스

| 이름 | 트랙 | HP | ATK | DEF |
|------|------|----|-----|-----|
| NEXUS Core | AI | 200 | 20 | 15 |
| Meltdown Titan | Nuclear | 190 | 22 | 12 |
| Storm Colossus | EcoTech | 180 | 18 | 14 |

---

## 리스크 목록

| # | 리스크 | 가능성 | 영향 | 대응 |
|---|--------|--------|------|------|
| 1 | BST 불균형 — 효율 점수가 비슷한 런에서 O(n) 퇴화 | 중 | 낮음 | 보고서에 "비균형 BST, 정렬 입력 시 O(n)" 한계로 명시 |
| 2 | 카드풀 고갈 — draw 연쇄 시 pool 빈 상태 드로우 시도 | 중 | 중간 | `CardPool::draw()` isEmpty 체크 후 조기 반환 구현 완료 |
| 3 | 보스 선택 고정 — 현재 마지막 등록 보스가 선택됨 | 중 | 낮음 | 데모용으로는 충분, 개선 시 보스 후보 배열 + 랜덤 선택 |
| 4 | 발표 시간 부족 — 런 완주에 시간 소요 | 중 | 중간 | `Constants.h`의 `MAP_DEPTH` 줄여 단축 런 준비 |
| 5 | 잘못된 입력 — 범위 밖 숫자·빈 입력 | 낮음 | 낮음 | 전 입력 검증 구현 완료, 발표 전 경계값 시연 |
