# 진행 상황 & 자료구조 분석 — Dungeon Explorer DS PBL

> 마지막 업데이트: 2026-06-10
> 기준 파일: `docs/RUBRIC.md`
> 분석 대상: `game/` 전체 소스 (헤더 + 구현)

이 문서는 **(1) 프로젝트에 사용된 모든 자료구조가 코드의 어느 부분에 어떻게 쓰였는지**를 전수 분석하고,
**(2) `docs/RUBRIC.md`의 모든 체크리스트 항목**을 코드 근거와 함께 충족 여부로 정리한다.

STL 컨테이너(`vector`, `list`, `stack`, `queue`, `map` 등)는 사용하지 않으며, 모든 자료구조는 `include/ds/`·`include/map/`에 직접 구현했다.

---

## 1. 자료구조 → 코드 사용처 매핑 (핵심)

| # | 자료구조 | 구현 파일 | 게임 내 사용처 (인스턴스) | 핵심 연산이 호출되는 위치 |
|---|----------|-----------|---------------------------|---------------------------|
| 1 | **LinkedList** (단일 연결 리스트) | `include/ds/LinkedList.h` | `CardPool`(공유 덱), `Hand`(손패), `Inventory`(포션) | `card/CardPool.cpp`, `run/Inventory.cpp` |
| 2 | **Stack** (LIFO) | `include/ds/Stack.h` | RunMap 이동 이력 `history`, Battle 카드 배정 이력 `assignStack`, DungeonGraph DFS 작업 스택 | `map/RunMap.cpp`, `battle/Battle.cpp`, `map/DungeonGraph.cpp` |
| 3 | **Queue** (FIFO) | `include/ds/Queue.h` | Enemy 행동 의도 `intentQueue` (순환 패턴) | `combatant/Enemy.cpp` |
| 4 | **DynamicArray** (가변 배열, ×2 증설) | `include/ds/DynamicArray.h` | Room 내 아이템 목록 `items`, 적 목록 `monsters` | `map/Room.cpp`, `map/DungeonGraph.cpp`(Room 보유) |
| 5 | **BST (ScoreTree)** | `include/ds/ScoreTree.h` + `src/ds/ScoreTree.cpp` | 전투 효율 점수 랭킹 트리 `scoreTree` | `run/Game.cpp` (`handleBattle`, `printRunSummary`) |
| 6 | **Graph (DungeonGraph)** | `include/map/DungeonGraph.h` + `src/map/DungeonGraph.cpp` | 27개 방의 4방향 인접 그래프 + 반복 DFS | `map/RunMap.cpp` (이동·DFS 출력), `run/Game.cpp` |
| 7 | **Sorting** (선택·삽입 정렬) | `include/ds/Sorting.h` + `src/ds/Sorting.cpp` | 인벤토리 가치순, 런 전투 통계·효율·딜량 순위 | `run/Inventory.cpp`, `run/Game.cpp` (`printRunSummary`) |

> 보조 고정 배열(필수 자료구조 아님): `StatusTracker`(상태이상), 각 Registry `entries[]`, `Event`의 `choices[]`/`outcomes[]`, `Battle`의 `party[]`/`enemies[]`, `Game::battleLog[]` 등은 크기 고정 배열로 구현해 핵심 자료구조와 역할을 분리했다.

---

## 2. 자료구조별 상세 분석

### 2-1. LinkedList — `include/ds/LinkedList.h`

`head`/`tail`/`count` 포인터를 유지하는 단일 연결 리스트 템플릿. 복사 생성자·대입 연산자·소멸자(`clear`)로 메모리 안전 보장.

| 게임 사용처 | 클래스 | 코드 |
|-------------|--------|------|
| 파티 공유 덱 | `CardPool` | `card/CardPool.h:10` `LinkedList<Card> cards;` |
| 손패 | `Hand` | `card/CardPool.h:24` `LinkedList<Card> cards;` |
| 포션 인벤토리 | `Inventory` | `run/Inventory.h:10` `LinkedList<Item> items;` |

- **추가**: `CardPool::addCard` / `Hand::addCard` → `pushBack` (`card/CardPool.cpp:6,30`), `Inventory::addItem` → `pushBack` (`run/Inventory.cpp:5`, `MAX_POTIONS` 초과 시 false).
- **제거/탐색**: `Hand::removeCard`·`CardPool::removeCard` → `removeAt` (`card/CardPool.cpp:10,32`), `Inventory::removeItem`/`findItem`은 `getHead()` 순회로 이름 검색 (`run/Inventory.cpp:15,27`). `LinkedList::find(value, outIndex)`도 제공.
- **엣지케이스**: 빈 리스트에서 `popFront`/`removeAt`/`getAt`이 모두 `false` 반환(`LinkedList.h:55,66,80`), 범위 밖 인덱스 차단.

### 2-2. Stack — `include/ds/Stack.h`

`topNode`/`count`의 단일 연결 LIFO. 복사 시 임시 배열로 순서 보존(`Stack.h:22`).

| 인스턴스 | 타입 | 역할 |
|----------|------|------|
| `RunMap::history` | `Stack<Position>` | 방 이동 Undo |
| `Battle::assignStack` | `Stack<AssignRecord>` | 카드 배정 Undo |
| `DungeonGraph::dfs` 지역변수 | `Stack<int>` | 반복식 깊이 우선 탐색 |

- **push**: `RunMap::move`/`advanceFloor`에서 `history.push(cur)` (`map/RunMap.cpp:104,119`), 카드 배정 시 `assignStack.push({ci, selected})` (`battle/Battle.cpp:479`).
- **pop**: `RunMap::undoMove` → `history.pop(prev)` (`map/RunMap.cpp:111`), 전투 중 `u/undo` → `assignStack.pop(rec)` 후 카드 손패 복귀(`battle/Battle.cpp:362`).
- **엣지케이스**: 빈 스택 `pop`/`peek` → `false`. 탐색 루프 `undoMove` 실패 시 "더 이상 되돌릴 수 없습니다" 출력(`Game.cpp:467`, `Battle.cpp:372`).

### 2-3. Queue — `include/ds/Queue.h`

`frontNode`/`rearNode`/`count` 단일 연결 FIFO.

- **인스턴스**: `Enemy::intentQueue` (`combatant/Enemy.h:13`).
- **enqueue**: `prepareIntent`이 다음 행동 카드를 적재(`combatant/Enemy.cpp:32-35`).
- **dequeue + 재적재(순환)**: `executeAndQueue`가 현재 의도를 꺼내 실행하고 다음 패턴 카드를 다시 enqueue → 적 행동 패턴이 무한 순환(`combatant/Enemy.cpp:37-41`). `peekIntent`으로 다음 행동을 전투 화면에 미리 공개(`Battle.cpp:273,283`).
- **엣지케이스**: 빈 큐 `dequeue`/`peek` → `false` (`Queue.h:45,56`), 스턴 시 의도만 소비(`Battle.cpp:542`).

### 2-4. DynamicArray — `include/ds/DynamicArray.h`

`data`/`sz`/`cap` 가변 배열. 용량 초과 시 `resize(cap==0?4:cap*2)`로 2배 증설(`DynamicArray.h:40`), 범위 밖 접근 시 `std::exit(1)`로 즉시 중단.

- **인스턴스**: `Room::items`, `Room::monsters` (`map/Room.h:16-17`).
- **추가/제거/접근**: `Room::addItem`/`addEnemy` → `pushBack`, `Room::takeItem` → `removeAt`(이름 매칭 후 제거, `map/Room.cpp:19-28`), `getItem`/`getEnemy` → `operator[]`, `clearEnemies` → `clear`.

### 2-5. BST — `include/ds/ScoreTree.h` + `src/ds/ScoreTree.cpp`

전투마다 "효율 점수 = 입힌 데미지 − 받은 데미지"를 키로 삽입하는 이진 탐색 트리.

- **삽입**: 포인터-투-포인터 순회로 위치 결정(`ScoreTree.cpp:18-25`).
- **중복 점수 정책**: `r.score < node.score`면 왼쪽, **그 외(동점 포함)는 오른쪽**에 삽입 → 중복 허용, 역중위 순회 시 동점이 연속 출력(`ScoreTree.cpp:21-22`).
- **출력**: `printDescending` → `printDesc`가 **역중위 순회(right → node → left)**로 내림차순 랭킹 출력(`ScoreTree.cpp:32-38`).
- **게임 연결**: `Game::handleBattle`이 전투 종료마다 `scoreTree.insert(...)` (`Game.cpp:142`), `printRunSummary`가 "전투 효율 랭킹"으로 출력(`Game.cpp:368`).
- **메모리**: `copyTree`(깊은 복사)·`clearHelper`(후위 순회 해제)로 안전.

### 2-6. Graph — `include/map/DungeonGraph.h` + `src/map/DungeonGraph.cpp`

`rooms[MAX_ROOMS]` + 인접 배열 `adjacency[MAX_ROOMS][4]`(방향당 이웃 id, −1=없음)로 표현한 방향 그래프.

- **구성**: `RunMap::buildGraph`가 3층×3행×3열 = **27개 방을 `addRoom`으로 등록**하고 상하좌우를 `connectRooms`로 연결(`map/RunMap.cpp:54-75`).
- **이동(실제 게임 통합)**: `RunMap::move`가 `graph.getNeighbor(현재방, 방향)`으로 다음 방 id를 얻어 이동, 이웃이 없으면(−1) "그 방향으로는 이동할 수 없습니다" 출력(`map/RunMap.cpp:98-102`).
- **DFS**: `DungeonGraph::dfs`는 **`Stack<int>` 기반 반복식 깊이 우선 탐색**으로 방문 배열을 채움(`DungeonGraph.cpp:42-62`). `printMap`이 DFS 방문 순서로 방-이웃 구조를 출력(`DungeonGraph.cpp:64-86`).
- **출력 연결**: 맵 탐색 중 `g/graph` 명령(`Game.cpp:452-457`)과 런 종료 요약 `printRunSummary`(`Game.cpp:341-343`)에서 `map.printGraph()` → DFS 결과를 보여준다.

### 2-7. Sorting — `include/ds/Sorting.h` + `src/ds/Sorting.cpp`

STL `sort` 대신 직접 구현한 정렬 3종.

| 함수 | 알고리즘 | 게임 사용처 |
|------|----------|-------------|
| `sortItemsByValueDescending` | **선택 정렬** | `Inventory::print` — 포션을 가치순 정렬 후 출력(`run/Inventory.cpp:53`) |
| `sortBattleStatsByDamage` | **삽입 정렬** | `Game::printRunSummary` — 런 전투를 입힌 데미지순 정렬(`Game.cpp:347`) |
| `sortScoresDescending` | **삽입 정렬** | `Game::printRunSummary` — 캐릭터 총 딜량·덱 카드 공격력 순위(`Game.cpp:376,407`) |

---

## 3. 복잡도 분석

| 자료구조 / 연산 | 시간 복잡도 | 근거 |
|------------------|-------------|------|
| LinkedList `pushBack`/`pushFront`/`popFront` | O(1) | head·tail 포인터 유지 |
| LinkedList `getAt`/`removeAt`/`find` | O(n) | 인덱스/값까지 순회 |
| Stack `push`/`pop`/`peek` | O(1) | top 노드 직접 접근 |
| Queue `enqueue`/`dequeue`/`peek` | O(1) | front·rear 포인터 유지 |
| DynamicArray `pushBack` | 분할상환 O(1) | 용량 초과 시 ×2 재할당 O(n) |
| DynamicArray `removeAt` / `operator[]` | O(n) / O(1) | 뒤 원소 shift / 인덱스 직접 접근 |
| BST `insert` | 평균 O(log n), 최악 O(n) | 불균형 시 한쪽으로 퇴화 |
| BST `printDescending` | O(n) | 모든 노드 1회 방문(역중위) |
| Graph `addRoom`/`connectRooms`/`getNeighbor` | O(1) | 인접 배열 직접 접근 |
| Graph `dfs` | O(V + E) | 방문 배열로 중복 방지 |
| Selection / Insertion Sort | O(n²) (삽입: 최선 O(n)) | 이중 루프 / 부분 정렬 시 단축 |

- 공간 복잡도: LinkedList·Stack·Queue·BST = O(n)(노드 수), DynamicArray = O(n), Graph = O(V²)(`MAX_ROOMS × 4` 인접 배열).

---

## 4. RUBRIC 체크리스트 충족 현황

### 4-1. 마일스톤 1 — 제안서 (`RUBRIC.md §2`)

| 항목 | 상태 | 근거 |
|------|------|------|
| 팀 이름 및 구성원 | ⚠️ | 보고서에 별도 명시 필요 (코드 외 항목) |
| 게임 제목 및 테마 | ✅ | "Dungeon Explorer / 지식의 상아탑" — 6트랙 카드 배틀 (`main.cpp`) |
| 초기 방 배치 스케치 | ✅ | `RunMap::LAYOUT[3][3][3]` 고정 레이아웃 (`map/RunMap.cpp:9-22`) |
| 아이템 목록 계획 | ✅ | `PotionType` 5종 + 상점 포션 7종 (`run/Item.h:6`, `Game.cpp:208-216`) |
| 몬스터/이벤트 목록 계획 | ✅ | 몬스터 15종(일반 12 + 보스 3), 이벤트 6종 |
| 자료구조 매핑 테이블 | ✅ | 본 문서 §1, `GAME_DESIGN.md` |
| 리스크 목록 | ✅ | `GAME_DESIGN.md` 하단 |

### 4-2. 마일스톤 2 — 선형 자료구조 (`RUBRIC.md §3`)

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| 연결 리스트 — 아이템 추가 | ✅ | `Inventory::addItem`/`CardPool::addCard` → `pushBack` |
| 연결 리스트 — 아이템 제거/탐색 | ✅ | `Inventory::removeItem`/`findItem`, `removeAt` (`run/Inventory.cpp`) |
| 빈 리스트 엣지케이스 처리 | ✅ | `popFront`/`removeAt` false 반환 (`LinkedList.h`) |
| 스택 — 이전 방 ID push | ✅ | `RunMap::move` → `history.push(cur)` |
| 스택 — undo 이동을 위한 pop | ✅ | `RunMap::undoMove` → `history.pop(prev)` |
| 빈 스택 엣지케이스 처리 | ✅ | `pop` false → "더 이상 되돌릴 수 없습니다" |
| 큐 — 게임 이벤트 enqueue | ✅ | `Enemy::prepareIntent` → `intentQueue.enqueue` |
| 큐 — 게임 이벤트 dequeue/처리 | ✅ | `Enemy::executeAndQueue` → dequeue 후 재적재(순환) |
| 스모크 테스트 시도 | ✅ | `make` 빌드 성공(경고 0, `ST_SLEEP` 미사용 경고만 잔존) |

### 4-3. 마일스톤 3 — 게임플레이 통합 (`RUBRIC.md §4`)

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| 클린 체크아웃에서 빌드 성공 | ✅ | `game/Makefile`, `make && ./dungeon_explorer` |
| `help` / `look` 명령 작동 | ✅ | 맵: `h/help`·`l/look` (`Game.cpp:445,450`), 전투: 동일 (`Battle.cpp:349,353`) |
| 방 간 이동 작동 | ✅ | `w/a/s/d` → `RunMap::move` → `graph.getNeighbor` |
| 잘못된 이동 방향 처리 | ✅ | `getNeighbor` −1 → 이동 차단 메시지 (`RunMap.cpp:99-102`) |
| 아이템 픽업 작동 | ✅ | 상점/이벤트 → `Inventory::addItem`, 전투 중 `use`로 사용 (`Battle.cpp:377`) |
| 인벤토리 출력 작동 | ✅ | `Inventory::print` (가치순 정렬 출력) |
| 이동 undo 작동 | ✅ | `u/undo` → `undoMove` → `history.pop` |
| 이벤트 최소 1개 — 상태 영향 | ✅ | 이벤트 6종, `OutcomeType` 8종 효과 (`Game::applyOutcome`) |

### 4-4. 마일스톤 4 — 고급 자료구조 (`RUBRIC.md §5`)

| 필수 증거 | 상태 | 근거 |
|-----------|------|------|
| BST 삽입 작동 | ✅ | `ScoreTree::insert` (`ScoreTree.cpp:18`) |
| BST 출력/탐색 작동 | ✅ | `ScoreTree::printDescending` (역중위) |
| 중복 점수 처리 정책 정의 | ✅ | 동점은 오른쪽 삽입 (`ScoreTree.cpp:21-22`), 본 문서 §2-5 |
| 그래프로 방 연결 표현 | ✅ | `DungeonGraph` 인접 배열, 27방 연결 (`RunMap::buildGraph`) |
| 맵 또는 인접 출력 작동 | ✅ | `printGraph`/`DungeonGraph::printMap` (DFS) — `g/graph`·런 요약 |
| 정렬 알고리즘 구현 | ✅ | 선택 1 + 삽입 2 (`Sorting.cpp`) |
| 정렬 결과 게임에서 확인 | ✅ | `printRunSummary`(통계/순위), `Inventory::print`(가치순) |
| 복잡도 분석 초안 존재 | ✅ | 본 문서 §3, `game/README.md` |

### 4-5. 최종 제출 체크리스트 (`RUBRIC.md §6`)

| 필수 제출물 | 상태 | 비고 |
|-------------|------|------|
| 소스코드 제출 | ✅ | `game/` |
| 빌드 방법 포함 | ✅ | `Makefile` + `md/README.md` |
| 빌드 성공 확인 | ✅ | `g++ -std=c++17 -Wall -Wextra -pedantic` 통과 |
| 최종 보고서 제출 | ❌ | 작성 필요 (§5 참고) |
| 데모 영상 또는 라이브 데모 | ❌ | 시나리오는 §6에 정리 |
| 팀 기여도 명세 포함 | ❌ | 작성 필요 |
| AI/외부 도움 공시 포함 | ❌ | 작성 필요 |

---

## 5. 최종 보고서용 자료 (`RUBRIC.md §7` 대응)

보고서 채점 항목별로 본 문서에서 바로 인용 가능한 위치:

| 보고서 항목 | 배점 | 본 문서 위치 |
|-------------|------|--------------|
| 프로젝트 명확한 설명 | 5 | 머리말 + §4-1 |
| 자료구조 매핑 테이블 정확성 | 10 | §1 |
| 연결 리스트·스택·큐 설명 | 10 | §2-1 ~ §2-3 |
| BST·그래프·정렬 설명 | 10 | §2-5 ~ §2-7 |
| 복잡도 분석 | 10 | §3 |
| 테스트 및 한계점 | 10 | §7 (한계점) |
| 기여도 명세 및 회고 | 5 | **팀 작성 필요** |

---

## 6. 데모 시나리오 (`RUBRIC.md §8` 대응)

빌드 무개입 실행 → 각 자료구조를 코드와 함께 시연하는 순서.

1. **빌드 개입 없이 실행**: `cd game && make && ./dungeon_explorer`.
2. **필수 명령 시연**: 파티 선택 → `h`(도움말)·`l`(현황)·`w/a/s/d`(이동)·`u`(undo)·`g`(그래프)·`i`(인벤토리).
3. **코드에서 자료구조 설명**:
   - LinkedList → `CardPool`/`Hand`/`Inventory` (전투 손패·덱·포션)
   - Stack → 맵 `u/undo`(이동 복귀), 전투 `u/undo`(카드 배정 취소)
   - Queue → 전투 화면 적 "다음:" 의도(`intentQueue` 순환)
   - DynamicArray → `Room`의 적/아이템 목록
   - BST → 런 종료 "전투 효율 랭킹"
   - Graph → `g/graph` 던전 DFS 출력
   - Sorting → 인벤토리 가치순, 런 요약 순위표
4. **엣지케이스 시연**: 더 이상 못 되돌리는 빈 스택, 벽 방향 이동 차단, 빈 인벤토리, 동점 효율 점수 처리.
5. **질문 대비**: "그래프는 어디에 쓰이나?" → `RunMap`이 `DungeonGraph::getNeighbor`로 모든 이동을 처리하고, DFS로 맵 구조를 출력.

---

## 7. 남은 작업 / 한계점

### 🔴 제출 필수 (코드 외)
- [ ] 최종 보고서 작성 — 매핑/복잡도/설명은 본 문서에서 인용, **테스트·한계·기여도·회고**만 추가
- [ ] 팀 기여도 명세 (담당 자료구조·기능)
- [ ] AI/외부 도움 공시

### 🟡 코드 품질 / 발표 보강 (선택)
- [ ] BST 균형: 현재 점수 분포에 따라 한쪽으로 퇴화 가능(최악 O(n)) — 한계점으로 명시하거나 균형 트리로 확장 여지
- [ ] 보스 선택: `handleBattle(true)`가 마지막 보스만 고정 선택(`Game.cpp:120-124`) — 3보스 중 랜덤화 가능
- [ ] `Room::takeItem`(방 바닥 아이템 픽업)은 구현되어 있으나 현재 맵 루프는 방에 아이템을 배치하지 않음 — 아이템 획득은 상점/이벤트 경유

### 한계점 (보고서용)
- BST는 비균형 이진 탐색 트리로 최악 O(n).
- 그래프는 인접 배열(O(V²) 공간)이며 `MAX_ROOMS=32` 고정.
- 정렬은 O(n²) 알고리즘(데이터 규모가 작아 실사용엔 충분).
- 입력 파싱은 첫 글자 기반 단순 처리(견고하나 다자릿수 음수 등은 비대상).

---

## 8. 완료된 주요 구현 (코드 확인 기준)

| 기능 | 파일 | 비고 |
|------|------|------|
| 캐릭터 로스터 6종 등록 | `registry/CharacterRoster.cpp` | 트랙별 1명(이민준/김서연/박준서/최유진/정하은/한승준), 3명 선택 |
| 카드 152장 등록 | `registry/CardRegistry.cpp` | 공용 31 + 6트랙 ×20~21 (스킬명 전부 한글화 완료) |
| 몬스터 15종 등록 | `registry/MonsterRegistry.cpp` | 일반 12 + 보스 3, `addCardByName` 카드명 한글 동기화 |
| 이벤트 6종 등록 | `registry/EventRegistry.cpp` | 강제 3(정전/노트북/상점) + 선택 3(연구노트/도서관/용액) |
| 전투 루프 (assign→execute→tick→reward) | `battle/Battle.cpp` | Stack 배정 Undo, `switch` 자리바꾸기, `use` 포션 |
| 공용 `resolveCard` / `applyEffect` | `battle/Battle.cpp` | 플레이어·적 공통 처리, `sourceIsPlayer` 분기 |
| Enemy 의도 Queue | `combatant/Enemy.cpp` | `Queue<Card>` 순환, intent 사전 공개 |
| 던전 맵 + 그래프 | `map/RunMap.cpp`, `map/DungeonGraph.cpp` | 3층 트리 진행 + 4방향 그래프 DFS |
| 런 종료 통계/랭킹 | `run/Game.cpp::printRunSummary` | DFS 맵 + 삽입정렬 통계 + BST 효율 랭킹 + 딜량/카드 순위 |
| 골드 + 상점 | `run/Game.cpp::handleShop` | 포션 7종/카드 구매, 인벤토리 교환 |
