# Dungeon Explorer — 개발자 문서

> 게임 기획 전체는 `GAME_DESIGN.md`, 자료구조 분석·루브릭 체크는 `PROGRESS.md` 참고.

---

## 빌드 & 실행

```bash
cd game
make                    # g++ -std=c++17 -Wall -Wextra -pedantic → ./dungeon_explorer
./dungeon_explorer
make clean
```

새 `src/*.cpp` 파일을 추가할 경우 `game/Makefile`의 해당 `*_SRC` 변수에 반드시 등록해야 링크된다.

---

## 파일 구조

```
game/
├── Makefile
├── include/
│   ├── core/
│   │   ├── Constants.h       모든 상수 — 수치 변경은 여기서만
│   │   ├── Track.h           트랙 enum (Nuclear/NewMaterial/Hydrogen/EcoTech/AI/Grid/None)
│   │   ├── Direction.h       방향 enum
│   │   └── UI.h              터미널 출력 헬퍼 (boxTop/Left/Center, hpBar, typewrite, displayWidth)
│   ├── effect/
│   │   ├── Effect.h          EffectType · EffectTarget · Effect 구조체 + namespace Effects 팩토리
│   │   └── StatusEffect.h    상태이상 트래커 (poison/stun/weaken/confuse/burn/vulnerable)
│   ├── card/
│   │   ├── Card.h            카드 클래스 (id, name, desc, track, effects[], bonusEffects[])
│   │   └── CardPool.h        LinkedList<Card> 공유 덱(CardPool) + 고정 슬롯 배열 손패(Hand)
│   ├── combatant/
│   │   ├── Combatant.h       HP·ATK·DEF·상태이상 베이스, permBoostAtk/Def 영구 스탯 부스트
│   │   ├── CombatantDef.h    스탯 구조체
│   │   ├── BattleCharacter.h 아군 캐릭터 (CharacterDef → BattleCharacter)
│   │   └── Enemy.h           적 (Queue<Card> intentQueue, 순환 패턴)
│   ├── battle/
│   │   ├── Battle.h          전투 루프 · Stack<AssignRecord> 카드 배정 Undo
│   │   └── BattleStats.h     전투 통계 (damageDealt / damageTaken / turns / charDamage[])
│   ├── event/
│   │   └── Event.h           Event · EventChoice · EventOutcome · OutcomeType
│   ├── map/
│   │   ├── Room.h            방 (DynamicArray<Item> items, DynamicArray<Enemy> monsters)
│   │   ├── RunMap.h          런 진행 맵 — Stack<Position> 이동 Undo, DungeonGraph 래핑
│   │   └── DungeonGraph.h    4방향 인접 배열 그래프 + Stack<int> 반복 DFS
│   ├── run/
│   │   ├── Game.h            런 전체 흐름 (selectParty → selectStartingDeck → run 루프)
│   │   ├── Item.h            포션 아이템 (PotionType, heal/atkUp/shield/poison/explosive)
│   │   └── Inventory.h       LinkedList<Item> 포션 인벤토리
│   ├── registry/
│   │   ├── CardRegistry.h    카드 등록·조회
│   │   ├── CharacterRoster.h 캐릭터 등록·조회
│   │   ├── MonsterRegistry.h 몬스터 등록·조회
│   │   ├── EventRegistry.h   이벤트 등록·조회
│   │   └── Registries.h      registerAll*() 선언 — main.cpp에서 한 번 호출
│   └── ds/
│       ├── LinkedList.h      단일 연결 리스트 (head·tail·count)
│       ├── Stack.h           LIFO 단일 연결 스택
│       ├── Queue.h           FIFO 단일 연결 큐
│       ├── DynamicArray.h    가변 배열 (×2 증설, 범위 초과 시 exit)
│       ├── ScoreTree.h       BST 랭킹 트리
│       └── Sorting.h         선택 정렬·삽입 정렬 선언
└── src/
    ├── main.cpp              타이틀 화면 → registerAll*() → Game::run()
    ├── core/Direction.cpp
    ├── effect/Effect.cpp · StatusEffect.cpp
    ├── card/Card.cpp · CardPool.cpp
    ├── combatant/Combatant.cpp · BattleCharacter.cpp · Enemy.cpp
    ├── battle/Battle.cpp
    ├── event/Event.cpp
    ├── map/RunMap.cpp · Room.cpp · RestRoom.cpp · DungeonGraph.cpp
    ├── run/Game.cpp · Item.cpp · Inventory.cpp
    ├── registry/CardRegistry.cpp · CharacterRoster.cpp
    │            MonsterRegistry.cpp · EventRegistry.cpp
    └── ds/ScoreTree.cpp · Sorting.cpp
```

---

## 게임 플로우

```
main()
 └─ registerAll*()          ← 전체 콘텐츠를 싱글턴 레지스트리에 등록
 └─ Game::Game()
     ├─ selectParty()        ← 로스터에서 3명 선택 (중복 불가)
     └─ selectStartingDeck() ← 공용 10장 + 트랙별 5장×3 = 25장 직접 선택
 └─ Game::run()
     └─ 루프: 파티 현황 출력 → 명령 입력 → w/a/s/d 이동 → 방 처리
         ├─ Battle  → handleBattle()
         ├─ Event   → handleEvent() → applyOutcome()
         ├─ Rest    → handleRest()  → HP/ATK/DEF 중 선택
         └─ Boss    → handleBattle(isBoss=true)
 └─ printRunSummary()       ← DFS 맵 + 정렬 통계 + BST 랭킹
```

---

## 명령어

### 맵 탐색

| 입력 | 동작 |
|------|------|
| `w` / `a` / `s` / `d` | 방 이동 (DungeonGraph::getNeighbor) |
| `l` / `look` | 맵 + 파티 현황 재출력 |
| `i` / `inv` | 인벤토리 확인 |
| `u` / `undo` | 이전 방으로 되돌리기 (Stack::pop) |
| `g` / `graph` | 던전 그래프 DFS 출력 |
| `h` / `help` | 명령어 목록 |

### 전투 카드 배정

매 턴 손패 5장 드로우, 캐릭터 3명에게 1장씩 배정. 손패 소진 시 자동 패스.  
`★전열` 캐릭터만 적의 공격을 받음.

| 입력 | 동작 |
|------|------|
| `0`~`n` | 해당 인덱스 카드를 현재 캐릭터에 배정 |
| `switch` | 자리 바꾸기 — 이후 두 캐릭터 인덱스 입력 (예: `0 1`) |
| `u` / `undo` | 직전 배정 취소 → 카드 원래 슬롯으로 복귀 (Stack 기반) |
| `use` | 포션 사용 — 인벤토리 번호 입력 |
| `l` / `look` | 전투 상황 + 손패 재출력 |
| `h` / `help` | 명령어 목록 |

---

## 자료구조 요약

STL 컨테이너 사용 금지. 모든 컨테이너는 `include/ds/` 또는 `include/map/`에 직접 구현.

| 자료구조 | 헤더 | 구현 | 게임 내 역할 |
|----------|------|------|-------------|
| LinkedList | `ds/LinkedList.h` | 헤더 전용 | `CardPool` 공유 덱, `Inventory` 포션 목록 |
| Stack | `ds/Stack.h` | 헤더 전용 | RunMap 이동 Undo, Battle 카드 배정 Undo, DungeonGraph DFS |
| Queue | `ds/Queue.h` | 헤더 전용 | Enemy 행동 패턴 순환 (`Queue<Card>`) |
| DynamicArray | `ds/DynamicArray.h` | 헤더 전용 | `Room` 내 아이템·적 목록 |
| ScoreTree (BST) | `ds/ScoreTree.h` | `ds/ScoreTree.cpp` | 전투 효율 점수 랭킹 |
| DungeonGraph | `map/DungeonGraph.h` | `map/DungeonGraph.cpp` | 4방향 인접 배열 그래프 + DFS |
| Sorting | `ds/Sorting.h` | `ds/Sorting.cpp` | 인벤토리·전투 통계·점수 정렬 |

> **Hand 클래스**: `CardPool.h`에 선언. LinkedList가 아닌 **고정 슬롯 배열 + `bool active[]`** 구조 — 카드 사용 시 슬롯 비활성화(인덱스 유지), Undo 시 `restoreCard(slotIdx, card)`로 정확한 슬롯 복귀.

---

## 복잡도 분석

| 자료구조 / 연산 | 시간 | 근거 |
|-----------------|------|------|
| LinkedList `pushBack` / `popFront` | O(1) | head·tail 포인터 유지 |
| LinkedList `getAt` / `removeAt` / `find` | O(n) | 인덱스까지 순회 |
| Stack / Queue `push·pop` / `enqueue·dequeue` | O(1) | top·front·rear 포인터 |
| DynamicArray `pushBack` | 분할상환 O(1) | 초과 시 ×2 재할당 |
| DynamicArray `removeAt` / `operator[]` | O(n) / O(1) | shift / 직접 접근 |
| BST `insert` | 평균 O(log n), 최악 O(n) | 비균형 시 퇴화 |
| BST `printDescending` | O(n) | 역중위 순회 |
| Graph `getNeighbor` / `connectRooms` | O(1) | 인접 배열 직접 접근 |
| Graph `dfs` | O(V + E) | 방문 배열로 중복 방지 |
| Selection / Insertion Sort | O(n²) | 이중 루프 / 최선 O(n) |

공간 복잡도: LinkedList·Stack·Queue·BST = O(n), DynamicArray = O(n), Graph = O(V²).

---

## 콘텐츠 추가 방법

### 카드 (`src/registry/CardRegistry.cpp`)

```cpp
using namespace Effects;

// 공용 카드 (Track::None)
Card c(id, "이름", "설명");
c.addEffect(attack(5));
reg.registerCard(c);

// 트랙 카드 — 트랙 일치 시 bonusEffect 추가 발동
Card c(id, "이름", "설명", Track::Grid);
c.addEffect(attack(8));
c.addBonusEffect(attack(4, EffectTarget::AllEnemies));
reg.registerCard(c);
```

**이펙트 팩토리 (`Effect.h` `namespace Effects`)**

| 팩토리 | 설명 |
|--------|------|
| `attack(v)` / `attack(v, AllEnemies)` | 물리 공격 (ATK 스케일, DEF 감소) |
| `handScaleAttack(v)` | 손패 수만큼 공격 반복 |
| `defense(v, dur)` / `defense(v, dur, Party)` | 방어막 (단일 / 파티 전체) |
| `heal(v)` / `heal(v, Party)` | HP 회복 |
| `draw(v)` | 다음 턴 추가 드로우 |
| `swap()` | 강제 파티 자리 바꾸기 (적 전용) |
| `atkUp/defUp(v, d)` | 버프 |
| `atkDown/defDown(v, d)` | 디버프 |
| `poison/stun/weaken/confuse(d)` | 상태이상 |

### 캐릭터 (`src/registry/CharacterRoster.cpp`)

```cpp
// CharacterDef(id, 이름, 설명, HP, ATK, DEF, Track)
roster.registerCharacter(CharacterDef(7, "이름", "설명", 110, 12, 6, Track::Grid));
```

### 몬스터 (`src/registry/MonsterRegistry.cpp`)

```cpp
EnemyDef def("이름", HP, ATK, DEF, Track);
addCardByName(def, cards, "타격");
addCardByName(def, cards, "독침");
reg.registerMonster(def);   // 패턴: 타격 → 독침 → 타격 → ...

// 보스
EnemyDef boss("보스 이름", HP, ATK, DEF, Track, true);
```

### 이벤트 (`src/registry/EventRegistry.cpp`)

```cpp
// 선택 이벤트 (복수 효과 가능)
Event e(id, "제목", "설명");
EventChoice c1("선택지 A");
c1.addOutcome({OutcomeType::AddCard,     1, "카드 1장 획득"});
c1.addOutcome({OutcomeType::DamageParty, 4, "파티 피해 4"});
e.addChoice(c1);
reg.registerEvent(e);

// 강제 이벤트
Event e(id, "제목", "설명", true);
EventChoice eff;
eff.addOutcome({OutcomeType::OpenShop, 0, "무료 보급"});
e.addChoice(eff);
reg.registerEvent(e);
```

**OutcomeType**

| 타입 | 효과 |
|------|------|
| `HealParty` | 파티 전체 HP 회복 |
| `DamageParty` | 파티 전체 피해 |
| `AddCard` | 무작위 카드 획득 |
| `RemoveSelectedCard` | 플레이어가 선택한 카드 제거 |
| `OpenShop` | 무료 보급 (카드 or 포션 1개 무료 선택) |
| `GainGold` | 미사용 (골드 시스템 제거됨) |
| `LoseGold` | 미사용 (골드 시스템 제거됨) |
| `Nothing` | 효과 없음 |

---

## TODO — 최종 제출 전

### 코드

- [x] 캐릭터 6종 등록 완료 (이민준/김서연/박준서/최유진/정하은/한승준)
- [x] 몬스터 15종 + 이벤트 6종 등록 완료
- [x] 전체 UI 영어 번역 완료
- [x] 빌드 경고 0

### 제출 문서

- [ ] 최종 보고서 (자료구조 설명·복잡도·테스트·한계·기여도·회고)
- [ ] 팀 기여도 명세
- [ ] AI/외부 도움 공시
- [ ] 데모 리허설 — 빌드 무개입 실행, 각 자료구조 코드에서 설명, 엣지케이스 시연

---

## 채점 기준 자가 점검

| 항목 | 배점 | 상태 |
|------|------|------|
| 자료구조 정확성 | 30 | LinkedList·Stack·Queue·DynamicArray·BST·Graph·Sorting 모두 직접 구현, 엣지케이스 처리, 소멸자 완비 |
| 자료구조 통합 | 25 | CardPool·Inventory(LinkedList), RunMap·Battle Undo(Stack), Enemy intent(Queue), Room(DynamicArray), ScoreTree(BST), 이동·DFS(Graph), 런 요약(Sorting) — 모두 실게임에서 호출됨 |
| 완성도 & 안정성 | 20 | 빌드 성공·경고 0, 입력 검증, 덱 빌딩 화면, 전투·이벤트·휴식·보스 완전 구현 |
| 코드 품질 & 모듈성 | 15 | include/src 분리, 레지스트리 기반 콘텐츠, 공용 resolveCard(), Constants.h 단일 상수 |
| 창의적 확장 & 발표 | 10 | 6트랙 시스템, 25장 덱 빌딩, 영구 스탯 부스트, 적 행동 카드명+설명 예고, 신소재 파티 방어막 |
