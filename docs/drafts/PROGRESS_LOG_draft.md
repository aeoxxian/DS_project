# Team Progress Log — 초안 (마일스톤별 4주차)

> 루브릭 마일스톤(제안서 → 선형 자료구조 → 게임플레이 통합 → 고급 자료구조)에 맞춰 4부로 작성.
> **날짜는 임시값**입니다 — 실제 주차 일정에 맞게 수정하세요(`<!-- 확인 -->`).

---

# Week 1 — 제안서 & 프로젝트 셋업 (Milestone 1)

## Team Information
- **Team name:** good
- **Week number:** 1
- **Date:** 2026-05-21

## Completed This Week
| Task | Member(s) | Evidence |
|---|---|---|
| 게임 콘셉트 확정 (6트랙 카드 배틀, 3층 던전, 보스) | 전원 | 제안서, `md/GAME_DESIGN.md` |
| 레포 구조 + Makefile + `Constants.h` 골격 | 김서정 | `game/` include/src 분리, `core/Constants.h` |
| 콘텐츠 레지스트리 골격 (Card/Character/Monster/Event) | 김현우, 정인강 | `registry/Registries.h` `registerAll()` |
| 캐릭터 로스터·트랙 enum 초안 | 김현우 | `registry/CharacterRoster.cpp`, `core/Track.h` |

## Problems Encountered
| Problem | Current Hypothesis | Next Step |
|---|---|---|
| STL 금지 범위가 모호 (`std::string`도 금지인가?) | 금지 대상은 컨테이너(vector/list/stack/queue/map)뿐, `std::string`은 허용 | 규칙 문서화 후 모든 컨테이너 직접 구현 착수 |
| 빌드 환경 제각각 | g++ -std=c++17 로 통일 | 공용 Makefile로 빌드 표준화 |

## Data Structures Updated
| Data Structure | Status | Notes |
|---|---|---|
| Linked list | Not started | 다음 주 CardPool/Inventory에 적용 예정 |
| Stack | Not started | 이동 Undo용 설계만 |
| Queue | Not started | 적 행동 패턴용 설계만 |
| BST | Not started | 효율 점수 키 정의 검토 |
| Graph | Not started | 27방 인접 설계 스케치 |
| Sorting | Not started | — |

## Plan for Next Week
| Task | Owner | Due Date |
|---|---|---|
| LinkedList 구현 + CardPool/Inventory 연결 | 김서정, 김현우 | Week 2 |
| Stack 구현 + 이동 Undo | 장재민 | Week 2 |
| Queue 구현 + 적 의도 큐 | 정인강 | Week 2 |

## Instructor Questions
1. 전투 효율 점수를 "입힌 피해 − 받은 피해"로 정의해도 BST 키로 적절한가요?
2. 크기 고정 보조 배열(레지스트리 entries 등)을 핵심 자료구조와 분리해 쓰는 것은 허용되나요?
3. 게임 텍스트가 한글/영어 혼용인데 채점에 영향이 있나요?

---

# Week 2 — 선형 자료구조 (Milestone 2)

## Team Information
- **Team name:** good
- **Week number:** 2
- **Date:** 2026-05-28

## Completed This Week
| Task | Member(s) | Evidence |
|---|---|---|
| LinkedList 구현(복사/대입/소멸자 포함) + 공유 덱·인벤토리 연결 | 김서정, 김현우 | `ds/LinkedList.h`, `card/CardPool.cpp`, `run/Inventory.cpp` |
| Stack 구현 + 방 이동 Undo | 장재민 | `ds/Stack.h`, `RunMap::history` |
| Queue 구현 + 적 행동 패턴 순환(의도 예고) | 정인강 | `ds/Queue.h`, `Enemy::intentQueue` |
| 빈 리스트/스택/큐 엣지케이스(false 반환) | 김서정 | `popFront`/`pop`/`dequeue` 가드 |
| 스모크 테스트(빌드 통과) | 전원 | `make` 경고 0 |

## Problems Encountered
| Problem | Current Hypothesis | Next Step |
|---|---|---|
| 손패에서 카드 사용 시 뒤 카드가 앞으로 밀려 인덱스가 깨짐 | 손패는 LinkedList 대신 고정 슬롯 배열 + `active[]`가 안정적 | Week 3에 `Hand` 재설계 적용 |
| Stack 복사 시 원소 순서가 뒤집힘 | 복사 생성 시 임시 배열로 순서 보존 필요 | Stack 복사 로직 수정 |

## Data Structures Updated
| Data Structure | Status | Notes |
|---|---|---|
| Linked list | Done | CardPool·Inventory에 통합, 엣지케이스 처리 |
| Stack | In progress | 이동 Undo 동작, 복사 순서 보존 수정 중 |
| Queue | In progress | 적 의도 enqueue/dequeue 순환 동작 |
| BST | Not started | — |
| Graph | Not started | — |
| Sorting | Not started | — |

## Plan for Next Week
| Task | Owner | Due Date |
|---|---|---|
| 던전 맵 이동 + 그래프 getNeighbor 통합 | 장재민 | Week 3 |
| 전투 루프(드로우/카드 배정/resolveCard) + Hand 재설계 | 김현우 | Week 3 |
| 이벤트 6종 + applyOutcome | 정인강 | Week 3 |

## Instructor Questions
1. 손패를 LinkedList 대신 고정 슬롯 배열로 바꿔도 "자료구조 통합" 점수에 문제가 없나요?
2. 적 행동 패턴 순환에 Queue를 쓰는 것이 "이벤트 처리" 요건을 충족하나요?

---

# Week 3 — 게임플레이 통합 (Milestone 3)

## Team Information
- **Team name:** good
- **Week number:** 3
- **Date:** 2026-06-04

## Completed This Week
| Task | Member(s) | Evidence |
|---|---|---|
| `w/a/s/d` 이동 + 그래프 `getNeighbor`로 모든 이동 처리 | 장재민 | `RunMap::move` → `DungeonGraph::getNeighbor` |
| 전투 루프(매턴 5장 드로우, 캐릭터별 카드 배정, 공용 `resolveCard`) | 김현우 | `battle/Battle.cpp` |
| 카드 배정 Undo(슬롯 인덱스 포함) + `Hand` 슬롯 재설계 | 김현우, 김서정 | `Stack<AssignRecord>`, `Hand::restoreCard` |
| 이벤트 6종 + `applyOutcome`(결과 FIFO 처리) | 정인강 | `EventRegistry.cpp`, `Game::handleEvent` |
| `help`/`look`, 잘못된 이동 차단, 인벤토리 출력 | 전원 | 맵·전투 명령 핸들러 |
| 클린 체크아웃 빌드 성공 | 전원 | `cd game && make` |

## Problems Encountered
| Problem | Current Hypothesis | Next Step |
|---|---|---|
| 골드 시스템이 흐름을 복잡하게 만들고 버그 유발 | 골드를 제거하고 "무료 보급(상점)"으로 단순화 | `gold` 멤버 삭제, `handleShop` 무료 선택으로 변경 |
| 보스가 항상 마지막 등록 보스로 고정 선택됨 | 보스 후보 배열 + 랜덤 선택이 필요 | 한계로 기록, 시간상 데모는 고정 보스로 진행 |

## Data Structures Updated
| Data Structure | Status | Notes |
|---|---|---|
| Linked list | Done | — |
| Stack | Done | 이동 Undo + 카드 배정 Undo(slotIdx) |
| Queue | Done | 적 의도 순환 + 이벤트 결과 FIFO |
| BST | In progress | 효율 점수 삽입 설계 착수 |
| Graph | In progress | 이동 연결 완료, DFS 출력 남음 |
| Sorting | Not started | — |

## Plan for Next Week
| Task | Owner | Due Date |
|---|---|---|
| BST(ScoreTree) + 런 요약 랭킹 | 김서정 | Week 4 |
| DungeonGraph DFS + `g/graph` 출력 | 장재민 | Week 4 |
| Sorting 3종 + 통계/인벤토리 정렬 | 김서정 | Week 4 |
| 몬스터/이벤트 최종 등록, UI 영어화 | 정인강, 김현우 | Week 4 |

## Instructor Questions
1. 그래프를 모든 방 이동에 직접 사용하면 "그래프 통합" 요건을 충족하나요?
2. 골드 제거(무료 보급) 같은 단순화가 "완성도" 점수에 불리하지 않나요?

---

# Week 4 — 고급 자료구조 & 마무리 (Milestone 4)

## Team Information
- **Team name:** good
- **Week number:** 4
- **Date:** 2026-06-10

## Completed This Week
| Task | Member(s) | Evidence |
|---|---|---|
| BST(ScoreTree) 효율 점수 삽입 + 동점 정책 + 런 요약 랭킹 | 김서정 | `ds/ScoreTree.cpp`, `Game::printRunSummary` |
| DungeonGraph DFS(`Stack<int>`) + `g/graph` 던전 구조 출력 | 장재민 | `map/DungeonGraph.cpp` |
| Sorting 3종(선택 1·삽입 2) + 인벤토리·전투 통계 정렬 | 김서정 | `ds/Sorting.cpp`, `Inventory::print` |
| 몬스터 15종/이벤트 6종 최종, 휴식 영구 부스트, UI 영어화 | 정인강, 김현우 | 레지스트리·`map/RestRoom.cpp` |
| 복잡도 분석 초안 + 경고 0 빌드 | 전원 | `md/PROGRESS.md §3`, `-Wall -Wextra -pedantic` |

## Problems Encountered
| Problem | Current Hypothesis | Next Step |
|---|---|---|
| BST 동점 점수 처리 정책 미정 | 동점은 오른쪽 삽입 → 역중위 순회 시 연속 출력 | 정책 확정 후 보고서에 명시 |
| 발표 중 런 완주 시간 부족 우려 | 맵 깊이를 줄인 단축 런이 필요 | `Constants.h` 조정한 데모용 빌드 준비 |

## Data Structures Updated
| Data Structure | Status | Notes |
|---|---|---|
| Linked list | Done | CardPool·Inventory |
| Stack | Done | 이동·배정 Undo, DFS |
| Queue | Done | 적 의도 순환, 이벤트 결과 |
| BST | Done | 효율 점수 랭킹, 동점 오른쪽 삽입 |
| Graph | Done | 27방 인접 + 이동 + DFS 출력 |
| Sorting | Done | 선택·삽입 정렬, 통계/인벤토리 |

## Plan for Next Week
| Task | Owner | Due Date |
|---|---|---|
| 최종 보고서(자료구조·복잡도·테스트·한계·회고) | 김서정 | 제출 전 |
| 팀 기여도 명세 + AI/외부 도움 공시 | 전원 | 제출 전 |
| 데모 리허설(빌드 무개입 실행, 자료구조별 코드 설명, 엣지케이스 시연) | 전원 | 발표 전 |

## Instructor Questions
1. 데모에서 7종 자료구조 코드 설명 분량을 어떻게 배분하면 좋을까요?
2. 비균형 BST·O(n²) 정렬을 "한계"로 적되 현재 규모에서 충분하다는 논거가 타당한가요?
