# Dungeon Explorer — 개발자 문서

게임 기획 및 시스템 설명은 `../GAME_DESIGN.md` 참고.

---

## 빌드

```bash
make
./dungeon_explorer
```

정리:

```bash
make clean
```

---

## 파일 구조

```
include/
├── core/
│   ├── Constants.h       모든 상수 (여기서만 수정)
│   ├── Track.h           트랙 enum
│   ├── Direction.h
│   └── UI.h              터미널 출력 유틸리티 (hpBar, clear, pause)
├── effect/
│   ├── Effect.h          이펙트 구조체 (EffectType, EffectTarget, Effect)
│   └── StatusEffect.h    상태이상 트래커
├── card/
│   ├── Card.h            카드 클래스
│   └── CardPool.h        LinkedList 기반 공유 덱(CardPool) + 손패(Hand)
├── combatant/
│   ├── Combatant.h       공통 베이스 (HP, 스탯, 상태이상)
│   ├── CombatantDef.h    스탯 구조체
│   ├── BattleCharacter.h 아군 캐릭터
│   └── Enemy.h           적 (Queue<Card> 의도 관리)
├── battle/
│   ├── Battle.h          전투 루프 (Stack<AssignRecord> 카드 배정 Undo)
│   └── BattleStats.h     전투 통계
├── event/
│   └── Event.h           이벤트 + 선택지 + 복수 결과
├── map/
│   ├── Room.h            방 (DynamicArray<Item/Enemy>)
│   ├── RunMap.h          트리형 런 맵 (Stack<int> 이동 Undo)
│   └── DungeonGraph.h    4방향 그래프 탐색 (Stack DFS)
├── run/
│   ├── Game.h            런 전체 흐름
│   ├── Item.h            아이템
│   └── Inventory.h       인벤토리 (LinkedList<Item>)
├── registry/
│   ├── CardRegistry.h
│   ├── CharacterRoster.h
│   ├── MonsterRegistry.h
│   ├── EventRegistry.h
│   └── Registries.h      registerAll*() 선언
└── ds/
    ├── LinkedList.h
    ├── Stack.h
    ├── Queue.h
    ├── DynamicArray.h
    ├── ScoreTree.h
    └── Sorting.h

src/
├── main.cpp              진입점 — 타이틀 화면 → registerAll*() → Game
├── core/Direction.cpp
├── effect/
├── card/
├── combatant/
├── battle/
├── event/
├── map/
├── run/
├── registry/             ← 콘텐츠 추가 시 이 디렉터리
└── ds/
    ├── ScoreTree.cpp     BST insert / printDescending 구현
    └── Sorting.cpp
```

---

## 게임 플로우 및 명령어

### 맵 탐색

| 입력 | 동작 |
|------|------|
| Enter | 다음 방으로 진행 |
| `l` / `look` | 맵 + 파티 현황 재출력 |
| `i` / `inv` | 인벤토리 확인 |
| `u` / `undo` | 이전 방으로 되돌리기 (Stack 기반) |
| `h` / `help` | 명령어 목록 |

### 전투 카드 배정

매 턴 손패 5장 드로우, 캐릭터 3명에게 각 1장씩 배정. 손패 소진 시 자동 패스.

`★전열` 캐릭터만 적의 공격을 받음. `switch` 명령으로 카드 대신 자리 바꾸기 선택 가능 (해당 캐릭터 행동 소비).

| 입력 | 동작 |
|------|------|
| `0`~`n` | 해당 인덱스 카드를 현재 캐릭터에 배정 |
| `switch` | 카드 대신 자리 바꾸기 — 두 캐릭터 인덱스 입력 (예: `0 1`) |
| `u` / `undo` | 직전 배정 취소 → 카드 손패 복귀 (자리 바꾸기 이후엔 불가) |
| `l` / `look` | 전투 상황 + 손패 재출력 |
| `h` / `help` | 명령어 목록 |

---

## 콘텐츠 추가 방법

### 카드 추가 (`src/registry/CardRegistry.cpp`)

```cpp
using namespace Effects;

// 공용 카드
Card c(id, "이름", "설명");
c.addEffect(attack(5));
reg.registerCard(c);

// 트랙 카드 — 트랙 일치 시 bonusEffect 추가 발동
Card c(id, "이름", "설명", Track::Grid);
c.addEffect(attack(8));
c.addBonusEffect(atkDown(3, 2));
reg.registerCard(c);
```

**이펙트 팩토리 함수 (`Effect.h` `namespace Effects`)**

| 팩토리 | 설명 |
|--------|------|
| `attack(v)` / `attack(v, AllEnemies)` | 공격 (ATK 스케일) |
| `handScaleAttack(v)` | 손패 수만큼 공격 반복 |
| `defense(v, d)` | 방어막 d턴 |
| `heal(v)` / `heal(v, Party)` | 회복 |
| `draw(v)` | 다음 턴 드로우 +v |
| `swap()` | 강제 자리 바꾸기 (적 전용) |
| `atkUp/defUp(v, d)` | 버프 |
| `atkDown/defDown(v, d)` | 디버프 |
| `poison/stun/weaken/confuse(d)` | 상태이상 |

### 캐릭터 추가 (`src/registry/CharacterRoster.cpp`)

```cpp
// CharacterDef(id, 이름, 설명, HP, ATK, DEF, Track)
r.registerCharacter(CharacterDef(1, "이름", "설명", 100, 10, 8, Track::AI));
```

### 몬스터 추가 (`src/registry/MonsterRegistry.cpp`)

카드 이름으로 CardRegistry에서 직접 재사용:

```cpp
EnemyDef def("이름", HP, ATK, DEF, Track);
addCardByName(def, cards, "Strike");
addCardByName(def, cards, "Poison Dart");
addCardByName(def, cards, "강제 교대");
reg.registerMonster(def);  // 패턴: Strike → Poison Dart → 강제 교대 → Strike → ...
```

보스:

```cpp
EnemyDef def("보스 이름", HP, ATK, DEF, Track, true);
addCardByName(def, cards, "Detonate");
// ...
reg.registerMonster(def);
```

### 이벤트 추가 (`src/registry/EventRegistry.cpp`)

선택 이벤트 (복수 효과 가능):

```cpp
Event e(id, "제목", "설명");
EventChoice c1("선택지 A");
c1.addOutcome({OutcomeType::AddCard,    1, "카드 1장 획득"});
c1.addOutcome({OutcomeType::DamageParty, 4, "파티 피해 4"});
e.addChoice(c1);
reg.registerEvent(e);
```

강제 이벤트:

```cpp
Event e(id, "제목", "설명", true);  // true = forced
EventChoice eff;
eff.addOutcome({OutcomeType::GainGold, 1, "전리품 +1"});
e.addChoice(eff);
reg.registerEvent(e);
```

**OutcomeType 목록**

| 타입 | 효과 |
|------|------|
| `HealParty` | 파티 전체 HP 회복 |
| `DamageParty` | 파티 전체 피해 |
| `AddCard` | 무작위 카드 획득 |
| `RemoveSelectedCard` | 플레이어가 선택한 카드 제거 |
| `GainGold` | 전리품 획득 |
| `LoseGold` | 전리품 감소 |
| `OpenShop` | 수상한 상점 오픈 |
| `Nothing` | 효과 없음 |

---

## 자료구조 구현 현황

STL 컨테이너(`vector`, `list`, `stack`, `queue`, `map` 등) 사용 금지.

| 자료구조 | 헤더 | 구현 파일 | 게임 내 역할 | 상태 |
|----------|------|-----------|-------------|------|
| LinkedList | `ds/LinkedList.h` | 헤더 전용 | CardPool, Hand, Inventory | ✅ 완료 |
| Stack | `ds/Stack.h` | 헤더 전용 | RunMap 이동 Undo, Battle 카드 배정 Undo, DungeonGraph DFS | ✅ 완료 |
| Queue | `ds/Queue.h` | 헤더 전용 | Enemy 의도 패턴 순환 (`Queue<Card>`) | ✅ 완료 |
| DynamicArray | `ds/DynamicArray.h` | 헤더 전용 | Room 내 아이템/적 목록 | ✅ 완료 |
| ScoreTree (BST) | `ds/ScoreTree.h` | `src/ds/ScoreTree.cpp` | 전투 효율 랭킹 | ✅ 완료 |
| Graph (DungeonGraph) | `map/DungeonGraph.h` | `src/map/DungeonGraph.cpp` | 던전 방 연결 + DFS 탐색 | ✅ 완료 |
| Sorting | `ds/Sorting.h` | `src/ds/Sorting.cpp` | 런 종료 전투 통계 정렬 | ✅ 완료 |

---

## 복잡도 분석

### LinkedList

| 연산 | 시간 복잡도 | 근거 |
|------|-------------|------|
| pushBack / pushFront | O(1) | tail 포인터 유지 |
| popFront | O(1) | head 직접 접근 |
| removeAt(i) | O(n) | i번째까지 순회 |
| getAt(i) | O(n) | i번째까지 순회 |
| size / isEmpty | O(1) | count 멤버 유지 |

### Stack / Queue

| 연산 | 시간 복잡도 |
|------|-------------|
| push / enqueue | O(1) |
| pop / dequeue | O(1) |
| peek | O(1) |

### DynamicArray

| 연산 | 시간 복잡도 | 근거 |
|------|-------------|------|
| pushBack | 분할상환 O(1) | 용량 초과 시 2배 재할당 O(n) |
| removeAt(i) | O(n) | 뒤 원소 shift |
| operator[] | O(1) | 인덱스 직접 접근 |

### ScoreTree (BST)

| 연산 | 시간 복잡도 | 근거 |
|------|-------------|------|
| insert | O(h) — 평균 O(log n), 최악 O(n) | 불균형 트리 시 퇴화 |
| printDescending (역중위) | O(n) | 모든 노드 방문 |
| 공간 복잡도 | O(n) | 노드 수만큼 동적 할당 |

중복 점수 처리: 동점은 오른쪽 서브트리에 삽입 (중복 허용, 역중위 순회 시 동점끼리 연속 출력).

### DungeonGraph (인접 배열)

| 연산 | 시간 복잡도 | 근거 |
|------|-------------|------|
| addRoom | O(1) | |
| connectRooms | O(1) | 배열 인덱스 직접 접근 |
| getNeighbor | O(1) | |
| dfs | O(V + E) | 방문 배열로 중복 방지 |
| 공간 복잡도 | O(V²) | MAX_ROOMS × 4 인접 배열 |

### Sorting

| 알고리즘 | 적용 함수 | 시간 복잡도 |
|----------|-----------|-------------|
| Selection Sort | `sortItemsByValueDescending` | O(n²) |
| Insertion Sort | `sortBattleStatsByDamage`, `sortScoresDescending` | 최선 O(n), 최악 O(n²) |

---

## TODO — 최종 제출 전 체크리스트

### 콘텐츠 등록

- [ ] **캐릭터 이름/스탯 확정** (`src/registry/CharacterRoster.cpp`) — 현재 플레이스홀더, 팀원별 이름·트랙·스탯으로 교체
- [x] **몬스터 카드 패턴 등록** — 일반 12종 + 보스 3종 모두 완료
- [x] **이벤트 등록** — 6개 이벤트 등록 완료 (강제 3 + 선택 3)

### 제출 문서

- [ ] **최종 보고서** — 자료구조 매핑 테이블, 복잡도 분석, 테스트 및 한계점, 기여도 명세, 회고
- [ ] **팀 기여도 명세** — 각 팀원이 담당한 자료구조 및 기능 명시
- [ ] **AI/외부 도움 공시** — 사용한 도구 및 범위 명시 (과제 요구사항)
- [ ] **데모 준비** — 빌드 무개입 실행, 각 자료구조 코드에서 설명, 엣지케이스 시연

---

## 채점 기준 자가 점검

루브릭 기준 현재 예상 점수 (100점 만점).

| 항목 | 배점 | 현재 상태 |
|------|------|-----------|
| 자료구조 정확성 | 30 | LinkedList·Stack·Queue·DynamicArray·BST·Graph·Sorting 전부 구현 완료 |
| 자료구조 통합 | 25 | CardPool·Hand·Inventory(LinkedList), moveHistory·assignStack(Stack), Enemy intent Queue\<Card\>, DFS(Stack), BattleStats 정렬, ScoreTree 랭킹 모두 실제 게임에서 사용 |
| 완성도 & 안정성 | 20 | 빌드 성공, 입력 검증, help/look/undo(맵·전투), 인벤토리, 카드 배정, 이벤트 6종, 몬스터 15종 모두 구현 |
| 코드 품질 & 모듈성 | 15 | 서브디렉터리 구조, 명확한 클래스 분리, 플레이어/몬스터 공통 resolveCard() 사용 |
| 창의적 확장 & 발표 | 10 | 트랙 시스템, 카드 배틀 구조 독창적. 발표 준비 필요 |
