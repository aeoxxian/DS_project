# Rubric and Checkpoints — 자가 점검 초안

> §1 채점 루브릭과 §7·§8(보고서/데모 루브릭)은 **강사 제공 기준이라 그대로 두고**, 마일스톤 체크리스트(§2~§6)의 **Complete? / Notes 칸만** 채웁니다.
> 근거는 `md/PROGRESS.md §4`(코드 근거 포함)에서 가져왔습니다.

---

## 1. Grading Rubric (강사 기준 — 변경 없음)

총 100점. 표 내용은 원본 유지(자료구조 정확성 30 / 통합 25 / 완성도 20 / 코드 품질 15 / 창의·발표 10).

---

## 2. Milestone 1: Proposal Checklist

| Item | Complete? | Notes |
|---|---|---|
| Team name and members | ✅ | good / 김서정·김현우·장재민·정인강 |
| Game title and theme | ✅ | Dungeon Explorer (지식의 상아탑) — 6트랙 카드 배틀 |
| Initial room layout sketch | ✅ | 3층×3×3=27방, 1층 배치 스케치 (제안서 Planned World) |
| Planned item list | ✅ | 포션 5종 + 영구 스탯 상승 + 트랙 카드 |
| Planned monster/event list | ✅ | 몬스터 15종(일반 12+보스 3), 이벤트 6종 |
| Data-structure mapping table | ✅ | 제안서 Data-Structure Mapping, `FINAL_REPORT.md §3` |
| Risk list | ✅ | 제안서 Risks and Plan (5개) |

*Suggested grading: 10 completion points or pass/revise.*

---

## 3. Milestone 2: Core Linear Structures Checklist

| Required Evidence | Complete? | Notes |
|---|---|---|
| Linked-list inventory can add items | ✅ | `Inventory::addItem` / `CardPool::addCard` → `pushBack` |
| Linked-list inventory can remove or find items | ✅ | `Inventory::removeItem`/`findItem`, `CardPool::removeCard` → `removeAt` |
| Empty-list edge case handled | ✅ | `popFront`/`removeAt`/`getAt` → `false` 반환 (`LinkedList.h`) |
| Stack can push previous room IDs | ✅ | `RunMap::move` → `history.push(cur)` |
| Stack can pop for undo movement | ✅ | `RunMap::undoMove` → `history.pop(prev)` |
| Empty-stack edge case handled | ✅ | 빈 스택 `pop` false → "Nothing to undo." |
| Queue can enqueue game events | ✅ | `Enemy::prepareIntent` → `intentQueue.enqueue` |
| Queue can dequeue/process game events | ✅ | `Enemy::executeAndQueue` → dequeue 후 재적재(순환) |
| Smoke tests attempted | ✅ | `make` 빌드 성공 — 경고 0 |

*Suggested grading: 15-20 points.*

---

## 4. Milestone 3: Gameplay Integration Checklist

| Required Evidence | Complete? | Notes |
|---|---|---|
| Game builds from clean checkout | ✅ | `cd game && make && ./dungeon_explorer` |
| `help` and `look` commands work | ✅ | 맵·전투 모두 `h/help`·`l/look` |
| Movement between rooms works | ✅ | `w/a/s/d` → `RunMap::move` → `graph.getNeighbor` |
| Invalid movement is handled | ✅ | `getNeighbor` −1 → 이동 차단 메시지 |
| Item pickup works | ✅ | 무료 보급/이벤트 → `Inventory::addItem`, 전투 중 `use` |
| Inventory display works | ✅ | `Inventory::print` — 선택 정렬 후 가치순 출력 |
| Undo movement works | ✅ | `u/undo` → `undoMove` → `history.pop` |
| At least one event affects output or player state | ✅ | 이벤트 6종, `OutcomeType` 효과 (`Game::applyOutcome`) |

*Suggested grading: 15-20 points.*

---

## 5. Milestone 4: Advanced Structures Checklist

| Required Evidence | Complete? | Notes |
|---|---|---|
| BST insertion works | ✅ | `ScoreTree::insert` (`src/ds/ScoreTree.cpp`) |
| BST display/search works | ✅ | `ScoreTree::printDescending` — 역중위 순회 |
| Duplicate-score policy is defined | ✅ | 동점은 오른쪽 삽입, 역중위 시 연속 출력 |
| Graph represents room connectivity | ✅ | `DungeonGraph` 인접 배열, 27방 연결 (`RunMap::buildGraph`) |
| Map or adjacency display works | ✅ | `g/graph` → `map.printGraph()` → DFS 방문 순서 |
| Sorting algorithm is implemented | ✅ | 선택 정렬 1 + 삽입 정렬 2 (`src/ds/Sorting.cpp`) |
| Sorting result is visible in the game or tests | ✅ | `printRunSummary` 통계·순위, `Inventory::print` 가치순 |
| Complexity analysis draft exists | ✅ | `md/PROGRESS.md §3`, `FINAL_REPORT.md §5` |

*Suggested grading: 15-20 points.*

---

## 6. Final Submission Checklist

| Required Artifact | Complete? | Notes |
|---|---|---|
| Source code submitted | ✅ | `game/` 전체 소스 |
| Build instructions included | ✅ | `Makefile` + `md/README.md` |
| Program builds successfully | ✅ | `g++ -std=c++17 -Wall -Wextra -pedantic` 경고 0 |
| Final report submitted | ✅ | `docs/FINAL_REPORT.md` (제출용 정리 필요) |
| Demo video or live demo completed | ✅ | 라이브 데모 완료 |
| Team contribution statement included | ✅ | 보고서 §8 기여도 명세(이번에 작성) |
| AI/external help disclosure included | ✅ | 보고서 §9 AI/외부 도움 공시 |

---

## 7. Final Report Rubric (강사 기준 — 변경 없음)

보고서 컴포넌트별 배점(설명 5 / 매핑 10 / 선형 설명 10 / 고급 설명 10 / 복잡도 10 / 테스트·한계 10 / 기여·회고 5).

---

## 8. Demo Rubric (강사 기준 — 변경 없음)

데모 컴포넌트별 배점(무개입 실행 10 / 명령 시연 15 / 코드 내 자료구조 설명 20 / 엣지케이스 10 / 질의응답 15).
