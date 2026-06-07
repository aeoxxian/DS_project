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
│   └── Enemy.h           적 (Queue<EnemySkill> 의도 관리)
├── battle/
│   ├── Battle.h          전투 루프 (Stack<AssignRecord> 카드 배정 Undo)
│   └── BattleStats.h     전투 통계
├── event/
│   └── Event.h           이벤트 + 선택지 + 결과
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

- 매 턴 손패 5장 드로우, 캐릭터 3명에게 각 1장씩 **필수** 배정 (스킵 불가)
- 손패가 소진된 경우에만 해당 캐릭터 자동 패스

| 입력 | 동작 |
|------|------|
| `0`~`n` | 해당 인덱스 카드를 현재 캐릭터에 배정 |
| `u` / `undo` | 직전 배정 취소 → 카드 손패 복귀 → 해당 캐릭터로 돌아가기 |
| `l` / `look` | 전투 상황 + 손패 재출력 |
| `h` / `help` | 명령어 목록 |

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
| LinkedList | `ds/LinkedList.h` | 헤더 전용 | CardPool, Hand, Inventory | ✅ 완료 |
| Stack | `ds/Stack.h` | 헤더 전용 | RunMap 이동 Undo, Battle 카드 배정 Undo, DungeonGraph DFS | ✅ 완료 |
| Queue | `ds/Queue.h` | 헤더 전용 | Enemy 의도 패턴 순환 | ✅ 완료 |
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

## TODO — 최종 제출 전 체크리스트

### 콘텐츠 등록

- [ ] **캐릭터 이름/스탯 확정** (`src/registry/CharacterRoster.cpp`) — 현재 "캐릭터1/2/3" 플레이스홀더, 팀원별 이름·트랙·스탯으로 교체
- [ ] **몬스터 등록 검토** (`src/registry/MonsterRegistry.cpp`) — 보스 포함 충분한 수
- [ ] **이벤트 등록 검토** (`src/registry/EventRegistry.cpp`) — 최소 1개 이상

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
| 자료구조 통합 | 25 | CardPool·Hand·Inventory(LinkedList), moveHistory·assignStack(Stack), Enemy intent(Queue), DFS(Stack), BattleStats 정렬, ScoreTree 랭킹 모두 실제 게임에서 사용 |
| 완성도 & 안정성 | 20 | 빌드 성공, 입력 검증, help/look/undo(맵·전투), 인벤토리, 아이템 픽업, 카드 필수 배정 모두 구현 |
| 코드 품질 & 모듈성 | 15 | 서브디렉터리 구조, 명확한 클래스 분리. 양호 |
| 창의적 확장 & 발표 | 10 | 트랙 시스템, 카드 배틀 구조 독창적. 발표 준비 필요 |
