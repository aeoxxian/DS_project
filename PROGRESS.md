# 진행 상황 체크 — Dungeon Explorer DS PBL

> 마지막 업데이트: 2026-06-08  
> 기준 파일: `docs/RUBRIC.md`

---

## 최종 채점 기준 예상 점수 (총 100점)

| 항목 | 배점 | 현재 상태 | 예상 점수 |
|------|------|-----------|-----------|
| 자료구조 정확성 | 30 | ✅ 7개 구조 모두 구현, 엣지케이스 처리 | **27–30** |
| 자료구조 통합 | 25 | ✅ 모두 게임 기능과 의미 있게 연결 | **22–25** |
| 완성도 & 안정성 | 20 | ⚠️ 빌드 성공, 시스템 완비 / 몬스터 스킬 미완성 | **14–18** |
| 코드 품질 & 모듈성 | 15 | ✅ include/src 분리, 명확한 클래스 설계 | **13–15** |
| 창의적 확장 & 발표 | 10 | ✅ 6트랙, 상태이상 시스템, 카드 150장 | **8–10** |
| **합계** | **100** | | **84–98** |

---

## 자료구조 구현 현황

| 자료구조 | 파일 | 핵심 연산 | 엣지케이스 | 게임 연결 |
|----------|------|-----------|------------|-----------|
| **LinkedList** | `include/ds/LinkedList.h` | pushBack/Front, popFront, removeAt, getAt, find, clear | ✅ | CardPool(공유 덱), Hand(손패) |
| **Stack** | `include/ds/Stack.h` | push, pop, peek, clear | ✅ | RunMap 이동 이력 (`moveHistory`) + DungeonGraph DFS |
| **Queue** | `include/ds/Queue.h` | enqueue, dequeue, peek, clear | ✅ | Enemy 행동 패턴 (`intentQueue`) |
| **BST** | `include/ds/ScoreTree.h` | insert, printDescending (역순 DFS) | ⚠️ 중복 처리 정책 명시 없음 | 런 종료 효율 점수 랭킹 |
| **Graph** | `include/map/DungeonGraph.h` | addRoom, connectRooms, getNeighbor, dfs, printMap | ✅ | 던전 맵, 방 연결 |
| **Sorting** | `include/ds/Sorting.h` | sortItemsByValueDescending, sortScoresDescending, sortBattleStatsByDamage | ✅ | 런 종료 전투 통계 정렬 |
| **DynamicArray** | `include/ds/DynamicArray.h` | pushBack, removeAt, resize(x2) | ✅ | Room 내 적/아이템 목록 |

---

## 마일스톤 1 — 제안서

| 항목 | 완료 | 비고 |
|------|------|------|
| 팀 이름 및 구성원 | ❌ | GAME_DESIGN.md에 없음 |
| 게임 제목 및 테마 | ✅ | 텍스트 기반 카드 배틀 게임 |
| 초기 방 배치 스케치 | ✅ | GAME_DESIGN.md 맵 구조 |
| 아이템 목록 계획 | ⚠️ | Inventory/Item 구조 있음, 목록 명세 없음 |
| 몬스터/이벤트 목록 계획 | ⚠️ | MonsterRegistry에 12마리+보스 있음, 이벤트 내용 미정 |
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
| 큐 — enqueue | ✅ | `Enemy::addSkill()` → `intentQueue.enqueue()` |
| 큐 — dequeue/처리 | ✅ | 적 행동 실행 후 자동 순환 |
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
| 이벤트 최소 1개 — 출력/상태 영향 | ✅ | `EventRegistry`, 5가지 OutcomeType |

---

## 마일스톤 4 — 고급 자료구조

| 필수 증거 | 완료 | 비고 |
|-----------|------|------|
| BST 삽입 작동 | ✅ | `ScoreTree::insert()` |
| BST 출력/탐색 작동 | ✅ | `ScoreTree::printDescending()` |
| 중복 점수 처리 정책 정의 | ⚠️ | `score >=` 이면 right로 → 중복 삽입 허용하나 **명시적 설명 없음** |
| 그래프로 방 연결 표현 | ✅ | `DungeonGraph` 인접 행렬 |
| 맵 또는 인접 출력 작동 | ✅ | `DungeonGraph::printMap()` |
| 정렬 알고리즘 구현 | ✅ | `Sorting.h` 3가지 함수 |
| 정렬 결과 게임에서 확인 가능 | ✅ | `Game::printRunSummary()` |
| 복잡도 분석 초안 존재 | ❌ | 없음 |

---

## 최종 제출 체크리스트

| 필수 제출물 | 완료 | 비고 |
|-------------|------|------|
| 소스코드 제출 | ✅ | |
| 빌드 방법 포함 | ✅ | Makefile |
| 빌드 성공 확인 | ✅ | |
| 최종 보고서 제출 | ❌ | |
| 데모 영상 또는 라이브 데모 | ❌ | |
| 팀 기여도 명세 포함 | ❌ | |
| AI/외부 도움 공시 포함 | ❌ | |

---

## 남은 주요 작업

### 🔴 필수 (감점 직결)
- [ ] **몬스터 스킬 추가** — `MonsterRegistry.cpp` 내 `// TODO: 스킬 추가` 14개  
  → 스킬 없으면 전투에서 적이 아무것도 안 함
- [ ] **캐릭터 이름/설명 완성** — "캐릭터1/2/3" 플레이스홀더 교체
- [ ] **이벤트 내용 등록** — `EventRegistry.cpp` 실제 이벤트 데이터 추가
- [ ] **BST 중복 처리 정책 명시** — 주석 또는 보고서에 설명 필요
- [ ] **복잡도 분석 초안 작성** — 각 자료구조 O(?) 정리
- [ ] **DungeonGraph를 게임 루프에 연결** — 현재 `RunMap`만 사용 중, `DungeonGraph`는 게임과 미연결 상태  
  → 맵 구조 완성 후 `Game::run()` 또는 `RunMap`에서 `DungeonGraph`를 실제로 활용하도록 연결 (발표 시 "그래프가 어디 쓰이나요?" 질문 대비)

### 🟡 보통 (제출 요건)
- [ ] 팀 이름 및 구성원 문서화
- [ ] 리스크 목록 작성 (GAME_DESIGN.md 또는 별도 문서)
- [ ] 최종 보고서 작성
- [ ] 팀 기여도 명세
- [ ] AI/외부 도움 공시

### 🟢 선택 (발표 품질)
- [ ] 보스 설계 완성 (NEXUS Core 등 스킬 패턴)
- [ ] `help`/`look`에 해당하는 인게임 명령 안내 보강
- [ ] 이동 undo를 루브릭에 맞게 설명하거나 대안 구현 검토
