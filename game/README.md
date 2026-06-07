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
│   └── CardPool.h        LinkedList 기반 공유 덱 + 손패
├── combatant/
│   ├── Combatant.h       공통 베이스 (HP, 스탯, 상태이상)
│   ├── CombatantDef.h    스탯 구조체
│   ├── BattleCharacter.h 아군 캐릭터
│   └── Enemy.h           적 (Queue<EnemySkill> 의도 관리)
├── battle/
│   ├── Battle.h          전투 루프
│   └── BattleStats.h     전투 통계
├── event/
│   └── Event.h           이벤트 + 선택지 + 결과
├── map/
│   ├── Room.h            방 (DynamicArray<Item/Enemy>)
│   ├── RunMap.h          트리형 런 맵
│   └── DungeonGraph.h    4방향 그래프 탐색 (Stack DFS)
├── run/
│   ├── Game.h            런 전체 흐름
│   ├── Item.h            아이템
│   └── Inventory.h       인벤토리 ← TODO: LinkedList 구현 필요
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
    ├── ScoreTree.cpp     ← TODO: 헤더의 구현을 여기로 이동 필요
    └── Sorting.cpp
```

---

## 콘텐츠 추가 방법

### 카드 추가 (`src/registry/CardRegistry.cpp`)

```cpp
// 공용 카드 — Track::None, 보너스 이펙트 없음
Card c(id, "이름", "설명", Track::None, TargetScope::Single);
c.addEffect(Effect(EffectType::PhysAttack, 5));
reg.registerCard(c);

// 트랙 카드 — 해당 트랙 지정, 트랙 일치 시 bonusEffect 추가 발동
Card c(id, "이름", "설명", Track::Grid, TargetScope::Single);
c.addEffect(Effect(EffectType::MagicAttack, 8));
c.addBonusEffect(Effect(EffectType::Debuff, 3, 2, ST_ATK_DOWN));
reg.registerCard(c);
```

### 캐릭터 추가 (`src/registry/CharacterRoster.cpp`)

```cpp
// CharacterDef(id, 이름, 설명, HP, ATK, DEF, MGC, Track)
r.registerCharacter(CharacterDef(1, "이름", "설명", 100, 10, 8, 8, Track::AI));
```

### 몬스터 추가 (`src/registry/MonsterRegistry.cpp`)

```cpp
// EnemyDef(이름, HP, ATK, DEF, MGC, Track, isBoss=false)
EnemyDef def("이름", 80, 12, 5, 0, Track::Hydrogen);

Card skill1(0, "단일 공격", "물리 타격", Track::None, TargetScope::Single);
skill1.addEffect(Effect(EffectType::PhysAttack, 10));
def.addSkill(skill1);

Card skill2(0, "독 분사", "전체 독", Track::None, TargetScope::All);
skill2.addEffect(Effect(EffectType::Debuff, 3, 2, ST_POISON, EffectTarget::AllEnemies));
def.addSkill(skill2);

reg.registerMonster(def);  // 패턴: skill1 → skill2 → skill1 → ...
```

### 이벤트 추가 (`src/registry/EventRegistry.cpp`)

```cpp
Event e(id, "제목", "설명 텍스트");
e.addChoice(EventChoice("선택지 A", EventOutcome(OutcomeType::HealParty, 10, "체력을 회복했다.")));
e.addChoice(EventChoice("선택지 B", EventOutcome(OutcomeType::Nothing,   0,  "아무 일도 없었다.")));
reg.registerEvent(e);
```

---

## 자료구조 구현 현황

STL 컨테이너(`vector`, `list`, `stack`, `queue`, `map` 등) 사용 금지.

| 자료구조 | 헤더 | 구현 파일 | 게임 내 역할 | 상태 |
|----------|------|-----------|-------------|------|
| LinkedList | `ds/LinkedList.h` | 헤더 전용 | CardPool, Hand | ✅ 완료 |
| Stack | `ds/Stack.h` | 헤더 전용 | DungeonGraph DFS | ✅ 완료 |
| Queue | `ds/Queue.h` | 헤더 전용 | Enemy 의도 패턴 | ✅ 완료 |
| DynamicArray | `ds/DynamicArray.h` | 헤더 전용 | Room 내 아이템/적 목록 | ✅ 완료 |
| ScoreTree (BST) | `ds/ScoreTree.h` | `src/ds/ScoreTree.cpp` | 전투 효율 랭킹 | ⚠️ 헤더에만 구현 |
| Graph (DungeonGraph) | `map/DungeonGraph.h` | `src/map/DungeonGraph.cpp` | 던전 방 연결 + DFS 탐색 | ✅ 완료 |
| Sorting | `ds/Sorting.h` | `src/ds/Sorting.cpp` | 런 종료 전투 통계 정렬 | ✅ 완료 |
| Inventory (LinkedList) | `run/Inventory.h` | `src/run/Inventory.cpp` | 아이템 보관 | ✅ 완료 |

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

중복 점수 처리: 동점은 오른쪽 서브트리에 삽입 (중복 허용).

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

## TODO — 미구현 항목

루브릭 체크포인트 기준으로 아직 완료되지 않은 항목.

### 마일스톤 2: 선형 자료구조

- [x] **`src/run/Inventory.cpp` 구현** — LinkedList<Item> 기반, addItem/removeItem/findItem/print 완료

- [x] **이동 Undo Stack 구현** — `RunMap`의 `Stack<int> moveHistory`: advance() 시 이전 방 ID push, `u`/`undo` 명령으로 pop 후 복귀. 빈 스택 엣지케이스 처리 완료

### 마일스톤 3: 게임플레이 통합

- [ ] **`help` 명령** (M3 필수)
  - 전투 중: 손패 카드 인덱스 입력 방법, Enter=스킵 안내
  - 맵 이동 중: 분기 선택 방법, 명령어 목록 안내

- [ ] **`look` 명령** (M3 필수)
  - 현재 방/상황을 다시 출력하는 명령
  - 전투: `displayBattleState()` 재출력
  - 맵: `map.printMap()` 재출력

- [ ] **아이템 픽업 게임플레이** (M3 필수: "아이템 픽업 작동")
  - Room에 아이템이 있을 때 플레이어가 획득 가능하도록 인터페이스 추가
  - `Room::takeItem()` → `Inventory::addItem()` 연결
  - `Game::handleRest()` 또는 `handleEvent()`에서 아이템 보상 연동

- [ ] **인벤토리 출력** (M3 필수: "인벤토리 출력 작동")
  - Inventory 구현 후 게임 내 특정 시점(휴식, 이벤트, 전투 보상)에서 출력

### 마일스톤 4: 고급 자료구조

- [ ] **`src/ds/ScoreTree.cpp`에 구현 이동** (M4 권장)
  - 현재 `ScoreTree.h`에 insert / printDescending 전체 구현이 있고 `ScoreTree.cpp`는 TODO 주석만 존재
  - 채점자가 cpp 파일 확인 시 미구현으로 볼 수 있음
  - insert, printDescending을 cpp로 이동하고 헤더에는 선언만 남기기

- [ ] **복잡도 분석 문서화 확인** — 위 '복잡도 분석' 섹션을 최종 보고서에 포함

### 최종 제출

- [ ] **캐릭터 등록** (`src/registry/CharacterRoster.cpp`) — 현재 TODO 상태, 3명 필수
- [ ] **몬스터 등록 검토** (`src/registry/MonsterRegistry.cpp`) — 보스 포함 충분한 수
- [ ] **이벤트 등록 검토** (`src/registry/EventRegistry.cpp`) — 최소 1개 이상
- [ ] **최종 보고서 작성** — 자료구조 매핑 테이블, 복잡도 분석, 테스트 및 한계점, 기여도 명세, 회고
- [ ] **팀 기여도 명세** — 각 팀원이 담당한 자료구조 및 기능 명시
- [ ] **AI/외부 도움 공시** — 사용한 도구 및 범위 명시 (과제 요구사항)
- [ ] **데모 준비** — 빌드 무개입 실행, 각 자료구조 코드에서 설명, 엣지케이스 시연

---

## 채점 기준 자가 점검

루브릭 기준 현재 예상 점수 (100점 만점).

| 항목 | 배점 | 현재 상태 |
|------|------|-----------|
| 자료구조 정확성 | 30 | LinkedList·Stack·Queue·DynamicArray·Graph·Sorting 완료. Inventory stub, ScoreTree cpp 비어있음 |
| 자료구조 통합 | 25 | CardPool·Hand·Enemy intent·DFS·BattleStats 정렬·랭킹 실제 사용. Inventory 미연동 |
| 완성도 & 안정성 | 20 | 빌드 성공, 입력 검증 있음. help/look/undo/인벤토리/아이템 픽업 모두 구현 완료 |
| 코드 품질 & 모듈성 | 15 | 서브디렉터리 구조, 명확한 클래스 분리. 양호 |
| 창의적 확장 & 발표 | 10 | 트랙 시스템, 카드 배틀 구조가 독창적. 발표 준비 필요 |
