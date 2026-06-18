# Dungeon Explorer (지식의 상아탑) — 최종 보고서

**과목**: 자료구조 PBL  
**게임 제목**: Dungeon Explorer / 지식의 상아탑  
**팀 이름**: <!-- 팀 이름 기입 -->  
**팀 구성원**: <!-- 이름 기입 -->  
**제출일**: 2026-06-17

---

## 1. 프로젝트 소개

*Dungeon Explorer / 지식의 상아탑*은 C++17로 구현한 텍스트 기반 터미널 카드 배틀 로그라이크 게임이다. Slay the Spire를 참조한 턴제 카드 전투, 3×3×3(27개 방) 던전 탐험, 6개 트랙 캐릭터 시스템을 결합했다.

이 프로젝트의 핵심 목표는 **STL 컨테이너를 일절 사용하지 않고** — `std::vector`, `std::list`, `std::stack`, `std::queue`, `std::map` 등 모두 금지 — 7가지 자료구조를 직접 구현해 게임 기능에 의미 있게 연결하는 것이다.

### 빌드 방법

```bash
cd game
make                  # g++ -std=c++17 -Wall -Wextra -pedantic
./dungeon_explorer
make clean
```

빌드는 경고 0개로 통과된다.

---

## 2. 최종 게임 설명

### 게임 흐름

```
main.cpp
  └─ registerAll()   모든 콘텐츠(카드 152장, 캐릭터 6종, 몬스터 15종, 이벤트 6종) 등록
  └─ Game::run()     한 런 전체 진행
       ├─ 캐릭터 3명 선택 + 덱 빌딩(공용 10장 + 트랙별 5장×3 = 25장)
       ├─ 3층 × 3행 × 3열 = 27개 방 던전 탐험 (w/a/s/d 이동, u/undo)
       ├─ 방 종류: Start / Battle / Event / Rest / Stairs / Boss
       └─ 런 종료 시 전투 통계·BST 랭킹·정렬 순위 출력
```

### 전투 시스템

매 턴 공유 덱에서 5장 드로우 → 캐릭터 3명에 카드 1장씩 배정(Undo 가능) → 플레이어·적 카드 실행. 전열(★) 캐릭터만 단일 적 공격을 받는다.

### 트랙 시스템

| 트랙 | 컨셉 |
|------|------|
| Nuclear | 독·디버프 장기전 |
| NewMaterial | 파티 방어막 탱커 |
| Hydrogen | 동결+폭발 순간 딜 |
| EcoTech | 파티 힐·서포트 |
| AI | 드로우 엔진 + CC |
| Grid | 광역 공격 딜러 |

---

## 3. 자료구조 매핑 테이블

| # | 자료구조 | 구현 파일 | 게임 내 인스턴스 | 핵심 연산 |
|---|----------|-----------|-----------------|-----------|
| 1 | **LinkedList** | `ds/LinkedList.h` | `CardPool::cards`, `Inventory::items` | `pushBack`, `removeAt`, `popFront`, 순회 |
| 2 | **Stack** | `ds/Stack.h` | `RunMap::history`, `Battle::assignStack`, DungeonGraph DFS 지역변수 | `push`, `pop`, `peek` |
| 3 | **Queue** | `ds/Queue.h` | `Game::handleEvent` (이벤트 결과 처리), `Enemy::intentQueue` (적 행동 패턴) | `enqueue`, `dequeue`, `peek` |
| 4 | **DynamicArray** | `ds/DynamicArray.h` | `Room::items`, `Room::monsters` | `pushBack`, `removeAt`, `operator[]`, `clear` |
| 5 | **BST (ScoreTree)** | `ds/ScoreTree.h` + `src/ds/ScoreTree.cpp` | `Game::scoreTree` | `insert`, `printDescending` |
| 6 | **Graph (DungeonGraph)** | `map/DungeonGraph.h` + `src/map/DungeonGraph.cpp` | `RunMap::graph` (27방, 4방향) | `addRoom`, `connectRooms`, `getNeighbor`, `dfs` |
| 7 | **Sorting** | `ds/Sorting.h` + `src/ds/Sorting.cpp` | 런 요약 통계, 인벤토리 출력 | 선택 정렬 1종, 삽입 정렬 2종 |

> **보조 배열** (필수 자료구조 아님): Registry `entries[]`, `Hand::slots[]`/`active[]`, `Battle::party[]`/`enemies[]` 등은 크기 고정 배열로 구현해 핵심 자료구조와 역할을 분리했다.

---

## 4. 자료구조 구현 설명

### 4-1. LinkedList — `include/ds/LinkedList.h`

`head` / `tail` / `count` 포인터를 유지하는 단일 연결 리스트 템플릿. 복사 생성자·대입 연산자·소멸자(`clear`)로 메모리 안전을 보장한다.

**게임 연결**

| 인스턴스 | 클래스 | 멤버 | 역할 |
|----------|--------|------|------|
| 파티 공유 덱 | `CardPool` | `LinkedList<Card> cards` | 드로우·반환 |
| 포션 인벤토리 | `Inventory` | `LinkedList<Item> items` | 포션 추가·제거·탐색 |

- `CardPool::addCard` / `Inventory::addItem` → `pushBack` (`MAX_POTIONS` 초과 시 false 반환)
- `CardPool::removeCard` → `removeAt`; `Inventory::removeItem` / `findItem` → `getHead()` 순회 후 이름 비교
- **Hand 재설계**: 카드 사용 시 인덱스 밀림 방지를 위해 손패(`Hand`)는 고정 슬롯 배열 + `bool active[]`로 구현. Undo 시 `restoreCard(slotIdx, card)`로 정확한 원래 슬롯에 복귀.

**엣지케이스**: 빈 리스트에서 `popFront` / `removeAt` / `getAt` 모두 `false` 반환, 범위 밖 인덱스 차단.

---

### 4-2. Stack — `include/ds/Stack.h`

`topNode` / `count` 단일 연결 LIFO. 복사 시 임시 배열로 순서를 보존한다.

**게임 연결**

| 인스턴스 | 타입 | 역할 |
|----------|------|------|
| `RunMap::history` | `Stack<Position>` | 방 이동 Undo |
| `Battle::assignStack` | `Stack<AssignRecord>` | 카드 배정 Undo (슬롯 인덱스 포함) |
| `DungeonGraph::dfs` 지역변수 | `Stack<int>` | 반복식 DFS |

- `RunMap::move` → `history.push(cur)` → `undoMove` → `history.pop(prev)`
- 카드 배정 시 `assignStack.push({charIdx, slotIdx, card})` → `u/undo` → `pop` → `hand.restoreCard(slotIdx, card)`
- **엣지케이스**: 빈 스택 `pop`·`peek` → `false`. `undoMove` 실패 시 "Nothing to undo." 출력.

---

### 4-3. Queue — `include/ds/Queue.h`

`frontNode` / `rearNode` / `count` 단일 연결 FIFO.

**게임 연결 1 — 이벤트 결과 처리** (`Game::handleEvent`, `run/Game.cpp`)

이벤트 방 진입 시 선택지의 모든 `EventOutcome`을 `Queue<EventOutcome>`에 enqueue한 뒤, dequeue로 꺼내 FIFO 순서대로 `applyOutcome()`을 호출한다. 결과가 여러 개인 선택지(예: 카드 획득 + 피해)에서 등록된 순서대로 효과가 보장된다.

```cpp
Queue<EventOutcome> outcomeQueue;
for (int i = 0; i < chosen.getOutcomeCount(); ++i)
    outcomeQueue.enqueue(chosen.getOutcome(i));   // 결과 적재

EventOutcome out;
while (outcomeQueue.dequeue(out))                 // FIFO 순서 처리
    applyOutcome(out);
```

**게임 연결 2 — 적 행동 패턴 순환** (`Enemy::intentQueue`)

- `prepareIntent()` → 다음 행동 카드 `enqueue`
- `executeAndQueue()` → 현재 의도 `dequeue` 후 실행, 다음 패턴 카드 `enqueue` (무한 순환)
- `peekIntent()` → 전투 화면에 **카드 이름 + 설명** 미리 공개

**엣지케이스**: 빈 큐 `dequeue`·`peek` → `false`. stun 상태에서 의도는 소비(패턴 진행)되지만 행동은 스킵.

---

### 4-4. DynamicArray — `include/ds/DynamicArray.h`

`data` / `sz` / `cap` 가변 배열. 용량 초과 시 `cap == 0 ? 4 : cap * 2`로 2배 증설. 범위 밖 접근 시 `std::exit(1)` 즉시 중단.

**게임 연결**: `Room::items`, `Room::monsters`

- `addItem` / `addEnemy` → `pushBack`
- `takeItem` → `removeAt`(이름 매칭 후 O(n) 시프트)
- `getItem` / `getEnemy` → `operator[]` O(1)
- `clearEnemies` → `clear` (전투 후 방 초기화)

---

### 4-5. BST (ScoreTree) — `include/ds/ScoreTree.h` + `src/ds/ScoreTree.cpp`

전투마다 "효율 점수 = damageDealt − damageTaken"을 키로 삽입하는 이진 탐색 트리.

**삽입**: 포인터-투-포인터 순회로 삽입 위치 결정.  
**중복 점수 정책**: `score < node.score` → 왼쪽, **그 외(동점 포함) → 오른쪽**. 역중위 출력 시 동점 연속 표시.  
**출력**: `printDescending` → 역중위 순회(right → node → left)로 내림차순 랭킹.

**게임 연결**:
- `Game::handleBattle` 전투 종료마다 `scoreTree.insert(score, battleLabel)`
- `Game::printRunSummary` 에서 "Battle efficiency ranking" 섹션으로 출력

**메모리**: `copyTree`(깊은 복사) · `clearHelper`(후위 순회 해제)로 안전.

---

### 4-6. Graph (DungeonGraph) — `include/map/DungeonGraph.h` + `src/map/DungeonGraph.cpp`

`rooms[MAX_ROOMS]` + `adjacency[MAX_ROOMS][4]` (방향당 이웃 id, −1 = 없음) 방향 그래프.

**구성**: `RunMap::buildGraph` — 3층×3행×3열 = 27개 방을 `addRoom`으로 등록, 상하좌우를 `connectRooms`로 연결.

**이동 통합**: `RunMap::move` → `graph.getNeighbor(현재방, 방향)`으로 다음 방 id 획득. 이웃 없으면(−1) 이동 차단 메시지 출력. 즉, **모든 이동이 그래프를 통해 처리**된다.

**DFS**: `DungeonGraph::dfs` — `Stack<int>` 기반 반복식 DFS로 방문 순서 결정. `printMap`이 DFS 결과로 방-이웃 구조 출력.

**UI 연결**: `g/graph` 명령 · `printRunSummary`에서 `map.printGraph()` 호출.

---

### 4-7. Sorting — `include/ds/Sorting.h` + `src/ds/Sorting.cpp`

STL `sort` 없이 직접 구현한 정렬 3종.

| 함수 | 알고리즘 | 게임 사용처 |
|------|----------|-------------|
| `sortItemsByValueDescending` | 선택 정렬 | `Inventory::print` — 포션 가치순 출력 |
| `sortBattleStatsByDamage` | 삽입 정렬 | `Game::printRunSummary` — 전투 입힌 데미지순 통계 |
| `sortScoresDescending` | 삽입 정렬 | `Game::printRunSummary` — 캐릭터 딜량·카드 공격력 순위 |

---

## 5. 핵심 알고리즘 및 복잡도 분석

### 시간 복잡도

| 자료구조 / 연산 | 시간 복잡도 | 근거 |
|-----------------|-------------|------|
| LinkedList `pushBack` / `popFront` | O(1) | head·tail 포인터 직접 접근 |
| LinkedList `getAt` / `removeAt` / find | O(n) | head부터 순회 |
| Stack `push` / `pop` / `peek` | O(1) | topNode 직접 접근 |
| Queue `enqueue` / `dequeue` / `peek` | O(1) | front·rear 포인터 직접 접근 |
| DynamicArray `pushBack` | 분할상환 O(1) | 초과 시 ×2 재할당 O(n) |
| DynamicArray `removeAt` | O(n) | 뒤 원소 앞으로 shift |
| DynamicArray `operator[]` | O(1) | 인덱스 직접 접근 |
| BST `insert` | 평균 O(log n), 최악 O(n) | 비균형 시 편향 트리로 퇴화 |
| BST `printDescending` | O(n) | 전 노드 1회 방문 (역중위) |
| Graph `addRoom` / `connectRooms` / `getNeighbor` | O(1) | 인접 배열 직접 인덱싱 |
| Graph `dfs` | O(V + E) | visited 배열로 중복 방지 |
| 선택 정렬 | O(n²) | 이중 루프 |
| 삽입 정렬 | 최선 O(n), 최악 O(n²) | 부분 정렬 시 조기 종료 |

### 공간 복잡도

| 자료구조 | 공간 복잡도 | 비고 |
|----------|-------------|------|
| LinkedList / Stack / Queue / BST | O(n) | 노드 수에 비례 |
| DynamicArray | O(n) | 할당 버퍼는 최대 2n |
| Graph | O(V²) | `MAX_ROOMS × 4` 인접 배열 |

### 데미지 계산 알고리즘

```
공격 출력 = (card.value + 공격자 ATK)
          × (100 − weaken%) / 100   [weaken 적용 시]
          × 150 / 100               [수비자 vulnerable 시]
실제 데미지 = max(0, 출력 − 수비자 DEF)
```

HandScaleAttack: 위 계산을 현재 손패 수만큼 반복.

---

## 6. 테스트 전략 및 결과

별도 테스트 프레임워크 없이 **빌드 경고 제로** + **수동 플레이스루** 방식으로 검증했다.

### 빌드 검증

```bash
cd game && make
# → 경고 0개 (-Wall -Wextra -pedantic)
```

### 자료구조별 수동 테스트

| 자료구조 | 정상 케이스 | 엣지케이스 |
|----------|-------------|-----------|
| LinkedList | 카드 추가·제거·순회 | 빈 리스트 `removeAt` → false |
| Stack | 이동 push/pop, 배정 push/pop | 빈 스택 pop → "Nothing to undo." |
| Queue | 적 의도 enqueue/dequeue/순환 | stun 시 dequeue 없이 패턴 진행 |
| DynamicArray | 방 내 몬스터·아이템 추가·제거 | 빈 방 `takeItem` → 오류 없이 종료 |
| BST | 전투마다 삽입, 런 종료 랭킹 출력 | 동점 삽입 → 오른쪽에 연속 삽입 |
| Graph | 27방 연결, 이동 시 getNeighbor | 벽 방향 이동 → "이동할 수 없습니다." |
| Sorting | 인벤토리 가치순, 전투 통계 딜순 | n=0 또는 n=1 입력 → 루프 무실행 |

### 명령 동작 확인

| 명령 | 결과 |
|------|------|
| `h` / `help` | 명령 목록 출력 |
| `l` / `look` | 현재 방·파티 상태 출력 |
| `w` / `a` / `s` / `d` | 4방향 이동 (그래프 getNeighbor) |
| `u` / `undo` | 이동 복귀 (Stack pop) |
| `g` / `graph` | DFS 던전 구조 출력 |
| `i` / `inventory` | 가치순 정렬 포션 목록 |
| 잘못된 방향 | 이동 차단 메시지 출력 |
| 빈 스택 undo | "Nothing to undo." 출력 |

---

## 7. 알려진 한계점

| # | 한계 | 원인 | 영향 |
|---|------|------|------|
| 1 | **BST 비균형** | 효율 점수가 비슷한 런에서 편향 트리 발생 가능 | insert 최악 O(n). AVL·RB-Tree 미구현. |
| 2 | **Graph 고정 크기** | `adjacency[MAX_ROOMS][4]` 정적 배열 | 방 수 확장 시 `MAX_ROOMS` 상수 변경 필요 — O(V²) 공간 |
| 3 | **정렬 O(n²)** | 선택·삽입 정렬 — 퀵/병합 정렬 미구현 | 현재 데이터 규모(최대 30개) 내에서 문제 없음 |
| 4 | **Hand 고정 슬롯** | `MAX_HAND_SIZE` 상수로 슬롯 수 고정 | 동적 확장 불가. 현재 설계에서는 불필요. |
| 5 | **보스 선택 고정** | `handleBattle(true)` → 마지막 등록 보스 항상 선택 | 3보스 중 랜덤화 미구현 |

---

## 8. 팀 기여도 명세

<!-- 팀원별 담당 작업 기입 -->

| 구성원 | 담당 영역 |
|--------|-----------|
|  |  |
|  |  |
|  |  |

---

## 9. AI 및 외부 도움 공시

이 프로젝트의 코드 설계·디버깅 과정에서 **Claude (Anthropic)** AI를 활용했다.  
- 활용 내용: 자료구조 엣지케이스 검토, 컴파일 에러 원인 분석, 게임 밸런스 수치 조정 제안  
- **모든 코드는 팀원이 직접 이해하고 작성·수정**했으며, AI 출력을 그대로 복붙하지 않았다.  
- 자료구조 구현(`ds/`, `map/DungeonGraph`) 및 게임 로직은 팀이 독립적으로 설계했다.

---

## 10. 회고

### 잘 된 점

- **자료구조 통합 완성도**: 7종 자료구조 모두 실제 게임 기능과 의미 있게 연결됐다. 특히 `DungeonGraph`가 **모든 이동 처리**를 담당하고, `Stack` 기반 DFS로 그래프를 탐색해 출력까지 구현한 점이 강점이다.
- **Undo 시스템**: 이동 Undo(`Stack<Position>`)와 카드 배정 Undo(`Stack<AssignRecord>`)를 슬롯 인덱스까지 포함해 구현함으로써 정확한 상태 복원을 달성했다.
- **경고 제로 빌드**: `-Wall -Wextra -pedantic` 조건에서 경고 없이 빌드됨.

### 개선할 점

- BST를 AVL 트리나 RB-Tree로 교체하면 최악 O(log n)을 보장할 수 있다.
- 정렬을 퀵소트·병합정렬로 개선하면 데이터 규모 확장 시 유리하다.
- 보스 랜덤 선택 로직을 추가하면 런마다 다른 도전이 가능하다.
- 단위 테스트 프레임워크(Google Test 등)를 도입하면 자료구조 엣지케이스를 자동화할 수 있다.
