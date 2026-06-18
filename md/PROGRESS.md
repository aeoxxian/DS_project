# 진행 상황 & 자료구조 분석 — Dungeon Explorer DS PBL

> 마지막 업데이트: 2026-06-18
> 기준 파일: `docs/RUBRIC.md`
> 분석 대상: `game/` 전체 소스 (헤더 + 구현)

이 문서는 **(1) 모든 자료구조가 코드의 어느 부분에 어떻게 쓰이는지** 전수 분석하고,
**(2) `docs/RUBRIC.md`의 체크리스트** 항목을 코드 근거와 함께 충족 여부로 정리한다.

STL 컨테이너(`vector`, `list`, `stack`, `queue`, `map` 등)는 사용하지 않으며,
모든 자료구조는 `include/ds/` · `include/map/`에 직접 구현했다.

---

## 1. 자료구조 → 코드 사용처 매핑

| # | 자료구조 | 구현 파일 | 게임 내 인스턴스 | 핵심 연산 호출 위치 |
|---|----------|-----------|-----------------|---------------------|
| 1 | **LinkedList** | `ds/LinkedList.h` | `CardPool::cards`, `Inventory::items` | `card/CardPool.cpp`, `run/Inventory.cpp` |
| 2 | **Stack** | `ds/Stack.h` | `RunMap::history`, `Battle::assignStack`, DFS 지역 스택 | `map/RunMap.cpp`, `battle/Battle.cpp`, `map/DungeonGraph.cpp` |
| 3 | **Queue** | `ds/Queue.h` | `Enemy::intentQueue`, `Game::handleEvent` `outcomeQueue` | `combatant/Enemy.cpp`, `run/Game.cpp` |
| 4 | **DynamicArray** | `ds/DynamicArray.h` | `Room::items`, `Room::monsters` | `map/Room.cpp` |
| 5 | **BST (ScoreTree)** | `ds/ScoreTree.h` + `src/ds/ScoreTree.cpp` | `Game::scoreTree` | `run/Game.cpp` |
| 6 | **Graph (DungeonGraph)** | `map/DungeonGraph.h` + `src/map/DungeonGraph.cpp` | `RunMap::graph` (27방 4방향 인접) | `map/RunMap.cpp`, `run/Game.cpp` |
| 7 | **Sorting** | `ds/Sorting.h` + `src/ds/Sorting.cpp` | (함수 직접 호출) | `run/Inventory.cpp`, `run/Game.cpp` |

> **보조 고정 배열** (필수 자료구조 아님): Registry `entries[]`, `Event::choices[]`, `Battle::party[]`/`enemies[]`, `Game::battleLog[]`, `Hand::slots[]`/`active[]` 등은 크기 고정 배열로 구현해 핵심 자료구조와 역할을 분리했다.

---

## 2. 자료구조별 상세 분석

### 2-1. LinkedList — `include/ds/LinkedList.h`

`head` / `tail` / `count` 포인터를 유지하는 단일 연결 리스트 템플릿.  
복사 생성자·대입 연산자·소멸자(`clear`)로 메모리 안전 보장.

| 게임 사용처 | 클래스 | 멤버 |
|-------------|--------|------|
| 파티 공유 덱 | `CardPool` | `LinkedList<Card> cards` (`card/CardPool.h`) |
| 포션 인벤토리 | `Inventory` | `LinkedList<Item> items` (`run/Inventory.h`) |

> **Hand 재설계**: 손패(`Hand`)는 카드 인덱스 안정성 문제로 LinkedList에서 **고정 슬롯 배열 + `bool active[]`** 구조로 교체했다. 카드 사용 시 슬롯을 비활성화만 하므로 뒤 카드가 앞으로 밀리지 않으며, Undo 시 `restoreCard(slotIdx, card)`로 정확한 원래 슬롯에 복귀한다 (`card/CardPool.h`, `card/CardPool.cpp`).

- **추가**: `CardPool::addCard` / `Inventory::addItem` → `pushBack` (`MAX_POTIONS` 초과 시 false).
- **제거/탐색**: `CardPool::removeCard` → `removeAt`; `Inventory::removeItem` / `findItem` → `getAt(i)` 루프 후 이름 비교 (Node는 private, `getHead()` 제거됨).
- **엣지케이스**: 빈 리스트에서 `popFront` / `removeAt` / `getAt` 모두 `false` 반환, 범위 밖 인덱스 차단.

---

### 2-2. Stack — `include/ds/Stack.h`

`topNode` / `count` 단일 연결 LIFO. 복사 시 임시 배열로 순서 보존.

| 인스턴스 | 타입 | 역할 |
|----------|------|------|
| `RunMap::history` | `Stack<Position>` | 방 이동 Undo |
| `Battle::assignStack` | `Stack<AssignRecord>` | 카드 배정 Undo (슬롯 인덱스 포함) |
| `DungeonGraph::dfs` 지역변수 | `Stack<int>` | 반복식 DFS |

- **push**: `RunMap::move` / `advanceFloor` → `history.push(cur)`;  
  카드 배정 시 `assignStack.push({ci, slotIdx, card})` — `slotIdx` 포함으로 정확한 슬롯 복귀 보장.
- **pop**: `RunMap::undoMove` → `history.pop(prev)`;  
  전투 `u/undo` → `assignStack.pop(rec)` → `hand.restoreCard(rec.slotIdx, rec.card)`.
- **엣지케이스**: 빈 스택 `pop` / `peek` → `false`. `undoMove` 실패 시 "Nothing to undo." 출력.

---

### 2-3. Queue — `include/ds/Queue.h`

`frontNode` / `rearNode` / `count` 단일 연결 FIFO.

**인스턴스 1 — `Enemy::intentQueue`** (`combatant/Enemy.h`):
- **enqueue**: `prepareIntent()` — 다음 행동 카드 적재.
- **dequeue + 재적재(순환)**: `executeAndQueue()` — 현재 의도 dequeue 후 실행, 다음 패턴 카드 enqueue. 패턴이 무한 순환.
- **peek 공개**: `peekIntent()` → 전투 화면에 **카드 이름 + 설명** 동시 표시 (`battle/Battle.cpp`).

**인스턴스 2 — `Queue<EventOutcome> outcomeQueue`** (`run/Game.cpp::handleEvent`):
- 선택된 이벤트 결과(`EventOutcome`)를 전부 **enqueue** 후 **dequeue** 순서대로 처리 → FIFO 의미론 명확히 활용.
- 선택지 1개에 복수 효과가 있을 때(예: 카드 획득 + 파티 피해) 순서 보장.

- **엣지케이스**: 빈 큐 `dequeue` / `peek` → `false`. stun 시 의도 소비(패턴 진행), 행동은 스킵.

---

### 2-4. DynamicArray — `include/ds/DynamicArray.h`

`data` / `sz` / `cap` 가변 배열. 용량 초과 시 `cap == 0 ? 4 : cap * 2`로 2배 증설.  
`operator[]` 범위 밖 접근: 빈 배열(`sz==0`)이면 `std::exit(1)`, 아니면 **클램프**(음수→0, 초과→sz-1)로 안전 접근 보장.

- **인스턴스**: `Room::items`, `Room::monsters` (`map/Room.h`).
- **추가**: `Room::addItem` / `addEnemy` → `pushBack`.
- **제거/접근**: `Room::takeItem` → `removeAt`(이름 매칭), `getItem` / `getEnemy` → `operator[]`, `clearEnemies` → `clear`.

---

### 2-5. BST — `include/ds/ScoreTree.h` + `src/ds/ScoreTree.cpp`

전투마다 "효율 점수 = damageDealt − damageTaken" 을 키로 삽입하는 이진 탐색 트리.

- **삽입**: 포인터-투-포인터 순회로 위치 결정 (`ScoreTree.cpp`).
- **중복 점수 정책**: `r.score < node.score` 이면 왼쪽, **그 외(동점 포함)는 오른쪽**에 삽입 → 중복 허용, 역중위 출력 시 동점 연속.
- **출력**: `printDescending` → 역중위 순회(right → node → left)로 내림차순 랭킹.
- **게임 연결**: `Game::handleBattle` 에서 전투 종료마다 `scoreTree.insert(...)`, `printRunSummary` 에서 "Battle efficiency ranking"으로 출력.
- **메모리**: `copyTree`(깊은 복사) · `clearHelper`(후위 순회 해제)로 안전.

---

### 2-6. Graph — `include/map/DungeonGraph.h` + `src/map/DungeonGraph.cpp`

`rooms[MAX_ROOMS]` + `adjacency[MAX_ROOMS][4]` (방향당 이웃 id, −1 = 없음) 방향 그래프.

- **구성**: `RunMap::buildGraph` — 3층×3행×3열 = **27개 방을 `addRoom`으로 등록**, 상하좌우를 `connectRooms`로 연결.
- **이동 통합**: `RunMap::move` → `graph.getNeighbor(현재방, 방향)` 으로 다음 방 id 획득. 이웃 없으면(−1) 이동 차단 메시지.
- **DFS**: `DungeonGraph::dfs` — **`Stack<int>` 기반 반복식 DFS**로 방문 순서 결정. `printMap`이 DFS 결과로 방-이웃 구조 출력.
- **출력 연결**: `g/graph` 명령 · `printRunSummary` 에서 `map.printGraph()` 호출 → DFS 결과 표시.

---

### 2-7. Sorting — `include/ds/Sorting.h` + `src/ds/Sorting.cpp`

STL `sort` 대신 직접 구현한 정렬 3종.

| 함수 | 알고리즘 | 게임 사용처 |
|------|----------|-------------|
| `sortItemsByValueDescending` | 선택 정렬 | `Inventory::print` — 포션 가치순 정렬 |
| `sortBattleStatsByDamage` | 삽입 정렬 | `Game::printRunSummary` — 전투 입힌 데미지순 |
| `sortScoresDescending` | 삽입 정렬 | `Game::printRunSummary` — 캐릭터 딜량·카드 공격력 순위 |

---

## 3. 복잡도 분석

| 자료구조 / 연산 | 시간 복잡도 | 근거 |
|-----------------|-------------|------|
| LinkedList `pushBack` / `popFront` | O(1) | head·tail 포인터 유지 |
| LinkedList `getAt` / `removeAt` / `find` | O(n) | 인덱스까지 순회 |
| Stack `push` / `pop` / `peek` | O(1) | topNode 직접 접근 |
| Queue `enqueue` / `dequeue` / `peek` | O(1) | front·rear 포인터 유지 |
| DynamicArray `pushBack` | 분할상환 O(1) | 초과 시 ×2 재할당 O(n) |
| DynamicArray `removeAt` | O(n) | 뒤 원소 shift |
| DynamicArray `operator[]` | O(1) | 인덱스 직접 접근 |
| BST `insert` | 평균 O(log n), 최악 O(n) | 비균형 시 퇴화 |
| BST `printDescending` | O(n) | 전 노드 1회 방문 (역중위) |
| Graph `addRoom` / `connectRooms` / `getNeighbor` | O(1) | 인접 배열 직접 접근 |
| Graph `dfs` | O(V + E) | 방문 배열로 중복 방지 |
| Selection Sort | O(n²) | 이중 루프 |
| Insertion Sort | 최선 O(n), 최악 O(n²) | 부분 정렬 시 단축 |

**공간 복잡도**: LinkedList·Stack·Queue·BST = O(n), DynamicArray = O(n), Graph = O(V²) (`MAX_ROOMS × 4` 인접 배열).

---

## 4. RUBRIC 체크리스트 충족 현황

### 4-1. 마일스톤 1 — 제안서

| 항목 | 상태 | 근거 |
|------|------|------|
| 팀 이름 및 구성원 | ⚠️ | 보고서에 별도 명시 필요 |
| 게임 제목 및 테마 | ✅ | "Dungeon Explorer / 지식의 상아탑" — 6트랙 카드 배틀 |
| 초기 방 배치 스케치 | ✅ | `RunMap::LAYOUT[3][3][3]` 고정 레이아웃 |
| 아이템 목록 계획 | ✅ | `PotionType` 5종 + 무료 보급 포션 7종 |
| 몬스터/이벤트 목록 계획 | ✅ | 몬스터 15종 (일반 12 + 보스 3), 이벤트 6종 |
| 자료구조 매핑 테이블 | ✅ | 본 문서 §1, `GAME_DESIGN.md` |
| 리스크 목록 | ✅ | `GAME_DESIGN.md` 하단 |

### 4-2. 마일스톤 2 — 선형 자료구조

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| 연결 리스트 — 아이템 추가 | ✅ | `Inventory::addItem` / `CardPool::addCard` → `pushBack` |
| 연결 리스트 — 아이템 제거/탐색 | ✅ | `Inventory::removeItem` / `findItem`, `CardPool::removeCard` → `removeAt` |
| 빈 리스트 엣지케이스 처리 | ✅ | `popFront` / `removeAt` / `getAt` → `false` 반환 (`LinkedList.h`) |
| 스택 — 이전 방 ID push | ✅ | `RunMap::move` → `history.push(cur)` |
| 스택 — undo 이동을 위한 pop | ✅ | `RunMap::undoMove` → `history.pop(prev)` |
| 빈 스택 엣지케이스 처리 | ✅ | `pop` false → "Nothing to undo." 출력 |
| 큐 — 게임 이벤트 enqueue | ✅ | `Enemy::prepareIntent` → `intentQueue.enqueue`; `handleEvent` → `outcomeQueue.enqueue` |
| 큐 — 게임 이벤트 dequeue/처리 | ✅ | `Enemy::executeAndQueue` → dequeue 후 재적재(순환); `handleEvent` → `outcomeQueue.dequeue` FIFO 처리 |
| 스모크 테스트 시도 | ✅ | `make` 빌드 성공 — 경고 0 |

### 4-3. 마일스톤 3 — 게임플레이 통합

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| 클린 체크아웃에서 빌드 성공 | ✅ | `cd game && make && ./dungeon_explorer` |
| `help` / `look` 명령 작동 | ✅ | 맵: `h/help` · `l/look`, 전투: 동일 |
| 방 간 이동 작동 | ✅ | `w/a/s/d` → `RunMap::move` → `graph.getNeighbor` |
| 잘못된 이동 방향 처리 | ✅ | `getNeighbor` −1 → 이동 차단 메시지 |
| 아이템 픽업 작동 | ✅ | 무료 보급/이벤트 → `Inventory::addItem`, 전투 중 `use`로 사용 |
| 인벤토리 출력 작동 | ✅ | `Inventory::print` — 선택 정렬 후 가치순 출력 |
| 이동 undo 작동 | ✅ | `u/undo` → `undoMove` → `history.pop` |
| 이벤트 최소 1개 — 상태 영향 | ✅ | 이벤트 6종, `OutcomeType` 6종 효과 (`Game::applyOutcome`) |

### 4-4. 마일스톤 4 — 고급 자료구조

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| BST 삽입 작동 | ✅ | `ScoreTree::insert` (`src/ds/ScoreTree.cpp`) |
| BST 출력/탐색 작동 | ✅ | `ScoreTree::printDescending` — 역중위 순회 |
| 중복 점수 처리 정책 정의 | ✅ | 동점은 오른쪽 삽입, 역중위 시 연속 출력 (본 문서 §2-5) |
| 그래프로 방 연결 표현 | ✅ | `DungeonGraph` 인접 배열, 27방 연결 (`RunMap::buildGraph`) |
| 맵 또는 인접 출력 작동 | ✅ | `g/graph` → `map.printGraph()` → DFS 방문 순서 출력 |
| 정렬 알고리즘 구현 | ✅ | 선택 정렬 1종 + 삽입 정렬 2종 (`src/ds/Sorting.cpp`) |
| 정렬 결과 게임에서 확인 | ✅ | `printRunSummary` 전투 통계·순위, `Inventory::print` 가치순 |
| 복잡도 분석 초안 존재 | ✅ | 본 문서 §3 |

### 4-5. 최종 제출 체크리스트

| 필수 제출물 | 상태 | 비고 |
|-------------|------|------|
| 소스코드 제출 | ✅ | `game/` |
| 빌드 방법 포함 | ✅ | `Makefile` + `md/README.md` |
| 빌드 성공 확인 | ✅ | `g++ -std=c++17 -Wall -Wextra -pedantic` 경고 0 |
| 최종 보고서 제출 | ✅ | `docs/FINAL_REPORT.md` 작성 완료 |
| 데모 영상 또는 라이브 데모 | ❌ | 시나리오 §6 참고 |
| 팀 기여도 명세 포함 | ❌ | 작성 필요 |
| AI/외부 도움 공시 포함 | ❌ | 작성 필요 |

---

## 5. 최종 보고서용 인용 위치

| 보고서 항목 | 배점 | 인용 위치 |
|-------------|------|-----------|
| 프로젝트 명확한 설명 | 5 | `GAME_DESIGN.md` 개요 + §4-1 |
| 자료구조 매핑 테이블 | 10 | 본 문서 §1 |
| LinkedList · Stack · Queue 설명 | 10 | 본 문서 §2-1 ~ §2-3 |
| BST · Graph · Sorting 설명 | 10 | 본 문서 §2-5 ~ §2-7 |
| 복잡도 분석 | 10 | 본 문서 §3 |
| 테스트 및 한계점 | 10 | 본 문서 §7 |
| 기여도 명세 및 회고 | 5 | **팀 작성 필요** |

---

## 6. 데모 시나리오

빌드 무개입 실행 → 각 자료구조를 코드와 함께 시연.

1. **빌드 개입 없이 실행**: `cd game && make && ./dungeon_explorer`
2. **파티·덱 선택**: 캐릭터 3명 선택 (중복 방지) → 공용 10장 + 트랙별 5장×3 = 25장 덱 빌딩
3. **필수 명령 시연**: `h`(도움말) · `l`(현황) · `w/a/s/d`(이동) · `u`(undo) · `g`(그래프) · `i`(인벤토리)
4. **코드에서 자료구조 설명**:
   - LinkedList → `CardPool::cards` (공유 덱), `Inventory::items` (포션)
   - Stack → 맵 `u/undo`(이동 복귀), 전투 `u/undo`(카드 배정 취소, 슬롯 복귀)
   - Queue → 전투 화면 적 "Next: [카드명] 설명" (`intentQueue` 순환)
   - DynamicArray → `Room::monsters` / `Room::items` (방 내 목록)
   - BST → 런 종료 "Battle efficiency ranking" (역중위 내림차순)
   - Graph → `g/graph` 던전 DFS 출력, `RunMap::move`가 `getNeighbor`로 이동 처리
   - Sorting → 인벤토리 가치순, 런 요약 전투 통계·점수 순위표
5. **엣지케이스 시연**: 빈 스택 undo 차단, 벽 방향 이동 차단, 빈 인벤토리, 동점 효율 점수 연속 출력
6. **질문 대비**: "그래프는 어디서 쓰이나?" → `RunMap`이 `DungeonGraph::getNeighbor`로 **모든 이동**을 처리하며, DFS로 맵 구조를 출력. "Hand는 왜 LinkedList가 아닌가?" → 카드 사용 시 인덱스 밀림 방지를 위해 고정 슬롯 배열로 교체, Undo 시 slotIdx로 정확한 슬롯 복귀.

---

## 7. 한계점 (보고서용)

- **BST 비균형**: 효율 점수 분포에 따라 한쪽으로 퇴화 가능 → 최악 O(n). 균형 트리(AVL/RB-Tree)는 미구현.
- **Graph 고정 크기**: 인접 배열 `MAX_ROOMS × 4` → O(V²) 공간. 방 수 확장 시 상수만 변경.
- **정렬 O(n²)**: 선택·삽입 정렬 — 데이터 규모가 작아(최대 30개) 실사용 충분.
- **Hand 배열 고정**: `MAX_HAND_SIZE` 상수로 슬롯 수 고정 — 동적 확장 불가(현재 설계에서 불필요).
- **보스 선택 고정**: `handleBattle(true)`가 마지막 등록 보스를 선택 — 3보스 중 랜덤화 미구현.

---

## 8. 완료된 주요 구현

| 기능 | 파일 | 비고 |
|------|------|------|
| 캐릭터 로스터 6종 | `registry/CharacterRoster.cpp` | 이민준/김서연/박준서/최유진/정하은/한승준, 3명 선택, 중복 방지 |
| 카드 152장 | `registry/CardRegistry.cpp` | 공용 31 + 트랙 6×20~21, 한글 스킬명 |
| 신소재 방어 파티 대상 | `registry/CardRegistry.cpp` | ID 202/205~208/211~214/217~219 → `EffectTarget::Party` |
| 몬스터 15종 (HP ~30% 감소) | `registry/MonsterRegistry.cpp` | 일반 12 + 보스 3, ATK·DEF 원본 유지 |
| 이벤트 6종 | `registry/EventRegistry.cpp` | 강제 3 (정전/노트북/보급) + 선택 3 (연구노트/도서관/용액) |
| 25장 덱 빌딩 화면 | `run/Game.cpp` `selectStartingDeck` | 공용 10 + 트랙별 5×3, 단계별 Undo |
| 카드 배정 Undo (슬롯 복귀) | `battle/Battle.cpp` | `Stack<AssignRecord>` — slotIdx 포함, `restoreCard` 사용 |
| 적 행동 카드명+설명 공개 | `battle/Battle.cpp` | `peekIntent` → "Next: [이름]  설명" |
| 휴식 영구 스탯 부스트 | `map/RestRoom.cpp` | HP +20 / ATK +1 / DEF +1 선택 (카드 강화 제거) |
| 골드 시스템 제거 → 무료 보급 | `run/Game.cpp` `handleShop` | 카드 or 포션 1개 무료 선택, `int gold` 멤버 삭제 |
| 던전 맵 + 4방향 그래프 DFS | `map/RunMap.cpp`, `map/DungeonGraph.cpp` | 3층 진행 + 현재 층만 표시 + `g/graph` DFS 출력 |
| 런 종료 통계·랭킹 | `run/Game.cpp` `printRunSummary` | DFS 맵 + 삽입정렬 전투 통계 + BST 효율 랭킹 + 딜량/카드 순위 |
| UI 한국어 통일 | `map/Room.cpp` 외 | 방 설명 등 모든 출력 한국어(UTF-8) |
