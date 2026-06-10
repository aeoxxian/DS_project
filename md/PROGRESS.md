# 진행 상황 체크 — Dungeon Explorer DS PBL

> 마지막 업데이트: 2026-06-10  
> 기준 파일: `docs/RUBRIC.md`

---

## 최종 채점 기준 예상 점수 (총 100점)

| 항목 | 배점 | 현재 상태 | 예상 점수 |
|------|------|-----------|-----------|
| 자료구조 정확성 | 30 | ✅ 7개 구조 모두 구현, 엣지케이스 처리 | **27–30** |
| 자료구조 통합 | 25 | ✅ 모두 게임 기능과 의미 있게 연결 (DungeonGraph 연결 미완) | **21–24** |
| 완성도 & 안정성 | 20 | ✅ 빌드 성공, 전투/이벤트/상점/맵 모두 작동 | **17–20** |
| 코드 품질 & 모듈성 | 15 | ✅ include/src 분리, 명확한 클래스 설계 | **13–15** |
| 창의적 확장 & 발표 | 10 | ✅ 6트랙, 상태이상 시스템, 카드 150+장 | **8–10** |
| **합계** | **100** | | **86–99** |

---

## 자료구조 구현 현황

| 자료구조 | 파일 | 핵심 연산 | 엣지케이스 | 게임 연결 |
|----------|------|-----------|------------|-----------|
| **LinkedList** | `include/ds/LinkedList.h` | pushBack/Front, popFront, removeAt, getAt, find, clear | ✅ | CardPool(공유 덱), Hand(손패), Inventory |
| **Stack** | `include/ds/Stack.h` | push, pop, peek, clear | ✅ | RunMap 이동 이력 (`moveHistory`), Battle 카드 배정 Undo (`assignStack`), DungeonGraph DFS |
| **Queue** | `include/ds/Queue.h` | enqueue, dequeue, peek, clear | ✅ | Enemy 행동 패턴 (`Queue<Card> intentQueue`) |
| **BST** | `include/ds/ScoreTree.h` | insert, printDescending (역순 DFS) | ✅ 중복은 오른쪽 삽입 | 런 종료 전투 효율 점수 랭킹 |
| **Graph** | `include/map/DungeonGraph.h` | addRoom, connectRooms, getNeighbor, dfs, printMap | ✅ | 구현 완료, Game 루프 연결 ❌ |
| **Sorting** | `include/ds/Sorting.h` | sortItemsByValueDescending, sortScoresDescending, sortBattleStatsByDamage | ✅ | 런 종료 전투 통계 정렬 (`printRunSummary`) |
| **DynamicArray** | `include/ds/DynamicArray.h` | pushBack, removeAt, resize(x2) | ✅ | Room 내 적/아이템 목록 |

---

## 마일스톤 1 — 제안서

| 항목 | 완료 | 비고 |
|------|------|------|
| 팀 이름 및 구성원 | ❌ | 문서에 없음 |
| 게임 제목 및 테마 | ✅ | 텍스트 기반 카드 배틀 게임 |
| 초기 방 배치 스케치 | ✅ | GAME_DESIGN.md 맵 구조 |
| 아이템 목록 계획 | ⚠️ | Inventory/Item 구조 있음, 아이템 목록 명세 없음 |
| 몬스터/이벤트 목록 계획 | ✅ | 몬스터 15종 + 이벤트 6종 모두 등록 완료 |
| 자료구조 매핑 테이블 | ✅ | GAME_DESIGN.md 하단 |
| 리스크 목록 | ✅ | GAME_DESIGN.md 하단 — 8개 리스크 |

---

## 마일스톤 2 — 선형 자료구조

| 필수 증거 | 완료 | 비고 |
|-----------|------|------|
| 연결 리스트 — 아이템 추가 | ✅ | `CardPool::addCard()` |
| 연결 리스트 — 아이템 제거/탐색 | ✅ | `CardPool::removeCard()`, `LinkedList::find()` |
| 빈 리스트 엣지케이스 처리 | ✅ | `popFront`, `removeAt` 모두 false 반환 |
| 스택 — push | ✅ | `RunMap::advance()` → `moveHistory.push(prevId)` |
| 스택 — pop | ✅ | `RunMap::undoMove()` → `moveHistory.pop(prevId)` |
| 빈 스택 엣지케이스 처리 | ✅ | `pop`/`peek` false 반환, "더 이상 되돌릴 수 없습니다" 출력 |
| 큐 — enqueue | ✅ | `Enemy::prepareIntent()` → `intentQueue.enqueue()` |
| 큐 — dequeue/처리 | ✅ | 적 행동 실행 후 자동 순환 (`executeAndQueue`) |
| 스모크 테스트 | ✅ | `make` 빌드 성공 |

---

## 마일스톤 3 — 게임플레이 통합

| 필수 증거 | 완료 | 비고 |
|-----------|------|------|
| 클린 체크아웃에서 빌드 성공 | ✅ | Makefile 포함 |
| `help` / `look` 명령 작동 | ✅ | `h/help` 도움말, `l/look` 맵+파티 상태 재출력 |
| 방 간 이동 작동 | ✅ | `RunMap` 분기 선택 |
| 잘못된 이동 방향 처리 | ✅ | `getNeighbor()` -1 반환 |
| 아이템 픽업 작동 | ✅ | `Inventory::addItem()` |
| 인벤토리 출력 작동 | ✅ | `inventory.print()` |
| 이동 undo 작동 | ✅ | `u/undo` 명령 → `RunMap::undoMove()` → `moveHistory.pop()` 이전 방 복귀 |
| 이벤트 최소 1개 — 출력/상태 영향 | ✅ | `EventRegistry`, 이벤트 6종, 8가지 OutcomeType |

---

## 마일스톤 4 — 고급 자료구조

| 필수 증거 | 완료 | 비고 |
|-----------|------|------|
| BST 삽입 작동 | ✅ | `ScoreTree::insert()` |
| BST 출력/탐색 작동 | ✅ | `ScoreTree::printDescending()` |
| 중복 점수 처리 정책 정의 | ✅ | 동점은 오른쪽 삽입 (중복 허용) — `README.md` 복잡도 분석 섹션에 명시 |
| 그래프로 방 연결 표현 | ✅ | `DungeonGraph` 인접 배열 (4방향) |
| 맵 또는 인접 출력 작동 | ✅ | `DungeonGraph::printMap()` |
| 정렬 알고리즘 구현 | ✅ | `Sorting.h` 3가지 함수 (Selection, Insertion x2) |
| 정렬 결과 게임에서 확인 가능 | ✅ | `Game::printRunSummary()` — 런 종료 시 자동 출력 |
| 복잡도 분석 초안 존재 | ✅ | `game/README.md` 복잡도 분석 섹션 (LinkedList, Stack/Queue, DynamicArray, BST, Graph, Sorting) |

---

## 최종 제출 체크리스트

| 필수 제출물 | 완료 | 비고 |
|-------------|------|------|
| 소스코드 제출 | ✅ | |
| 빌드 방법 포함 | ✅ | Makefile |
| 빌드 성공 확인 | ✅ | `make` 성공, 경고 없음 |
| 최종 보고서 제출 | ❌ | |
| 데모 영상 또는 라이브 데모 | ❌ | |
| 팀 기여도 명세 포함 | ❌ | |
| AI/외부 도움 공시 포함 | ❌ | |

---

## 남은 주요 작업

### 🔴 필수 (감점 직결)

- [ ] **캐릭터 이름/설명 완성** — `src/registry/CharacterRoster.cpp`의 "캐릭터1/2/3" 플레이스홀더를 팀원별 이름·트랙·스탯으로 교체
- [ ] **DungeonGraph를 게임 루프에 연결** — 현재 `RunMap`만 사용, `DungeonGraph` 인스턴스가 `Game`에 없음  
  → 발표 시 "그래프가 어디 쓰이나요?" 질문 대비 필요  
  → 옵션: `Game`에 `DungeonGraph` 멤버 추가, `handleRest()`나 `printRunSummary()`에서 `printMap()` 호출

### 🟡 보통 (제출 요건)

- [ ] 팀 이름 및 구성원 문서화
- [ ] 최종 보고서 작성 (자료구조 매핑, 복잡도 분석, 테스트/한계, 기여도, 회고)
- [ ] 팀 기여도 명세
- [ ] AI/외부 도움 공시

### 🟢 선택 (발표 품질)

- [ ] `help`/`look` 인게임 명령 안내 보강
- [ ] 보스 선택 로직 개선 — 현재 마지막 보스(`Storm Colossus`)만 등장, 3보스 중 랜덤 선택 추가 가능
- [ ] 발표 시나리오 준비 — 각 자료구조 코드 설명 + 엣지케이스 시연 순서 정리

---

## 완료된 주요 구현 (코드 확인 기준)

| 기능 | 파일 | 비고 |
|------|------|------|
| 전투 시스템 (assign → execute → reward) | `src/battle/Battle.cpp` | Stack 기반 카드 배정 Undo 포함 |
| `switch` 명령 (자리 바꾸기) | `Battle::assignPhase()` | 카드 대신 행동 소비, 자리 교환 |
| `resolveCard()` 공용 메서드 | `Battle::resolveCard()` | 플레이어/몬스터 공통 카드 효과 처리 |
| Party 타겟 아군/적 분리 | `Battle::applyEffect()` | `sourceIsPlayer` 기준 Defense·Buff 분기 |
| Enemy `Queue<Card>` 의도 시스템 | `src/combatant/Enemy.cpp` | 순환 패턴, intent 미리 공개 |
| 몬스터 15종 등록 | `src/registry/MonsterRegistry.cpp` | 일반 12 + 보스 3, `addCardByName()` 헬퍼 사용 |
| 이벤트 6종 등록 | `src/registry/EventRegistry.cpp` | 강제 3 + 선택 3, 복수 결과(addOutcome) 지원 |
| Gold 추적 + 상점 | `src/run/Game.cpp` | `int gold`, `handleShop()` |
| ScoreTree BST 랭킹 | `src/ds/ScoreTree.cpp` | 역중위 순회, 동점 오른쪽 삽입 |
| 복잡도 분석 문서화 | `game/README.md` | LinkedList~Sorting 전 자료구조 분석 포함 |
| 카드 150+장 등록 | `src/registry/CardRegistry.cpp` | 공용 + 6트랙별 트랙 카드 |
