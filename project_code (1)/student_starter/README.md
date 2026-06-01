# Dungeon Explorer — 개발자 문서

게임 기획 및 시스템 설명은 `../../GAME_DESIGN.md` 참고.

---

## 빌드

```bash
# Make
make
./dungeon_explorer

# CMake
mkdir build && cd build
cmake .. && cmake --build .
./dungeon_explorer
```

## 테스트

```bash
make test
```

---

## 파일 구조

```
include/
├── Constants.h           모든 상수 (여기서만 수정)
├── Track.h               트랙 enum
├── CardEffect.h          이펙트 구조체 (5종)
├── StatusEffect.h        상태이상 키 + StatusTracker
├── Card.h                카드 클래스
├── Hand.h                손패
├── BattleCharacter.h     아군 캐릭터
├── Enemy.h               적
├── Battle.h              전투 루프
├── Event.h               이벤트 + 선택지
├── EventOutcome.h        이벤트 결과 타입
├── MapNode.h             맵 노드 (트리 노드)
├── RunMap.h              트리형 맵 (그래프 아님)
├── RestRoom.h            휴식 방
├── Run.h                 런 전체 흐름
└── ds/
    ├── CardPool.h
    ├── CardRegistry.h
    ├── CharacterRoster.h
    ├── MonsterRegistry.h
    └── EventRegistry.h

src/
├── main.cpp              진입점 — registerAll*() 후 Run::start()
├── CardRegistry.cpp      ← 카드 추가 시 여기만
├── CharacterRoster.cpp   ← 캐릭터 추가 시 여기만
├── MonsterRegistry.cpp   ← 몬스터 추가 시 여기만
├── EventRegistry.cpp     ← 이벤트 추가 시 여기만
└── ...
```

---

## 콘텐츠 추가 방법

### 카드 추가 (`src/CardRegistry.cpp`)
```cpp
// 공용 카드 (ID 1~99) — 보너스 이펙트 없음
Card c(id, "이름", "설명", CardType::Normal);
c.addEffect({EffectType::Attack, 5});
reg.registerCard(c);

// 트랙 카드 (ID 100+) — 트랙 일치 시 보너스 이펙트 추가 발동
Card c(id, "이름", "설명", CardType::TrackCard, Track::Grid);
c.addEffect({EffectType::Attack, 8});           // 항상 발동
c.addBonusEffect({EffectType::Debuff, 3, 2, "atk_down"}); // 트랙 일치 시만
reg.registerCard(c);
```

### 캐릭터 추가 (`src/CharacterRoster.cpp`)
```cpp
r.registerCharacter(CharacterDef(id, "이름", "설명", baseHp, Track::AI));
```

### 몬스터 추가 (`src/MonsterRegistry.cpp`)
```cpp
reg.registerMonster(Enemy("이름", maxHp, attackPower, Track::Hydrogen));
```

### 이벤트 추가 (`src/EventRegistry.cpp`)
```cpp
Event e(id, "제목", "설명 텍스트");
e.addChoice(EventChoice("선택지 A", EventOutcome(OutcomeType::HealParty, 10, "체력을 회복했다.")));
e.addChoice(EventChoice("선택지 B", EventOutcome(OutcomeType::Nothing,   0,  "아무 일도 없었다.")));
reg.registerEvent(e);
```

---

## TODO (자료구조 구현 필요)

- `src/Inventory.cpp` — 연결 리스트
- `include/ds/Stack.h` — 이동 기록 / undo
- `include/ds/Queue.h` — 게임 이벤트 처리
- `src/DungeonGraph.cpp` — 던전 이동
- `src/ScoreTree.cpp` — BST 점수 관리
- `src/Sorting.cpp` — 아이템 정렬

STL 컨테이너(`vector`, `list`, `stack`, `queue`, `map` 등) 사용 금지.
