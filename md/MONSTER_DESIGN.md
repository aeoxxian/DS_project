# Monster Design

## Overview

이 문서는 Dungeon Explorer의 몬스터 설계를 정리한다. 몬스터는 별도의 AI 스크립트가 아니라, `EnemyDef`에 등록된 카드 목록을 순서대로 사용하는 전투 개체다. 전투 중에는 다음 행동을 `Queue<Card>`에 넣어 플레이어에게 예고하고, 실행 후 다음 카드를 다시 큐에 넣는다.

핵심 목표는 다음과 같다.

- 몬스터를 데이터 중심으로 등록한다.
- 각 몬스터는 HP, ATK, DEF, Track, Boss 여부, 행동 카드 목록을 가진다.
- 전투에서는 다음 행동 1개만 공개한다.
- 행동 패턴은 등록된 카드 순서대로 순환한다.
- 일반 전투는 1~2마리, 보스 전투는 보스 1마리를 배치한다.

## Related Files

| File | Role |
|------|------|
| `game/include/registry/MonsterRegistry.h` | `EnemyDef`, `MonsterRegistry` 선언 |
| `game/src/registry/MonsterRegistry.cpp` | 몬스터 실제 등록 데이터 |
| `game/include/combatant/Enemy.h` | 전투용 Enemy 클래스 선언 |
| `game/src/combatant/Enemy.cpp` | 행동 큐, 패턴 순환, intent 처리 |
| `game/src/battle/Battle.cpp` | 몬스터 행동 실행 및 전투 효과 적용 |
| `game/src/run/Game.cpp` | 일반 전투/보스 전투 몬스터 선택 |
| `game/include/core/Constants.h` | 몬스터 관련 최대 크기 상수 |

## Data Model

### EnemyDef

`EnemyDef`는 몬스터 등록용 데이터 구조다.

| Field | Type | Description |
|------|------|-------------|
| `name` | `std::string` | 몬스터 이름 |
| `baseHP` | `int` | 최대 체력 |
| `baseAttack` | `int` | 기본 공격력 |
| `baseDefend` | `int` | 기본 방어력 |
| `track` | `Track` | 몬스터 속성/테마 |
| `isBoss` | `bool` | 보스 여부 |
| `cards` | `Card[MAX_ENEMY_SKILLS]` | 행동 카드 목록 |
| `cardCount` | `int` | 등록된 행동 카드 수 |

`EnemyDef`는 `CombatantDef`를 상속하므로, 캐릭터와 같은 기본 전투 스탯 체계를 공유한다.

### Enemy

`Enemy`는 실제 전투에서 사용되는 몬스터 객체다.

| Field | Type | Description |
|------|------|-------------|
| `cards` | `Card[MAX_ENEMY_SKILLS]` | 이 몬스터가 사용할 카드 패턴 |
| `cardCount` | `int` | 카드 개수 |
| `patternIndex` | `int` | 다음에 사용할 카드 인덱스 |
| `intentQueue` | `Queue<Card>` | 다음 행동을 저장하는 큐 |

등록 데이터인 `EnemyDef`가 전투에 들어갈 때 `Enemy(def)`로 변환된다.

## Constants

| Constant | Value | Meaning |
|------|------:|---------|
| `MAX_ENEMIES` | 4 | 한 전투에 등장 가능한 최대 적 수 |
| `MAX_ENEMY_SKILLS` | 8 | 몬스터 1마리가 가질 수 있는 최대 행동 카드 수 |
| `MAX_MONSTER_REGISTRY_SIZE` | 128 | 등록 가능한 몬스터 정의 최대 개수 |

## Monster Registry

`MonsterRegistry`는 정적 싱글턴 레지스트리다. 게임 시작 시 `registerAllMonsters()`에서 모든 몬스터를 등록한다.

주요 함수는 다음과 같다.

| Function | Description |
|------|-------------|
| `registerMonster(def)` | 몬스터 정의를 배열에 추가 |
| `getAt(index, out)` | 인덱스로 몬스터 조회 |
| `findByName(name, out)` | 이름으로 몬스터 조회 |
| `size()` | 등록된 몬스터 수 반환 |
| `printAll()` | 등록된 몬스터 목록 출력 |

몬스터의 행동 카드는 `addCardByName()`을 통해 `CardRegistry`에서 이름으로 찾아 등록한다. 카드 이름이 없으면 콘솔에 missing card 메시지를 출력하고 해당 카드는 추가하지 않는다.

## Registered Monsters

### Normal Monsters

HP는 원본 대비 약 30% 감소. ATK/DEF는 원본 그대로.

| Name | Track | HP | ATK | DEF | Cards |
|------|------|---:|----:|----:|-------|
| Fission Hound | Nuclear | 40 | 8 | 4 | Strike, Poison Dart, Fission Burst |
| Reactor Wraith | Nuclear | 65 | 12 | 6 | Radiation Pulse, Containment Breach, Guard |
| Nano Swarm | NewMaterial | 35 | 7 | 5 | Double Strike, Disarm, Nano Blade |
| Carbon Golem | NewMaterial | 80 | 10 | 14 | Composite Shield, Graphene Edge, Heavy Blow |
| Cryo Crawler | Hydrogen | 40 | 9 | 5 | Cryo Jet, Disarm, Strike |
| Plasma Leech | Hydrogen | 55 | 11 | 6 | Fuel Cell Blast, Plasma Arc, Combustion |
| Gust Wraith | EcoTech | 30 | 8 | 4 | Gust Slash, Smoke Veil, Wind Shear |
| Turbine Beast | EcoTech | 60 | 13 | 8 | Turbine Kick, Tailwind, Heavy Blow |
| Rogue Drone | AI | 40 | 9 | 5 | Algorithm Strike, Data Wipe, Predictive Aim |
| Neural Phantom | AI | 65 | 11 | 6 | Neural Surge, Recursive Strike, Hotfix |
| Static Fiend | Grid | 45 | 8 | 6 | Blackout Strike, Static Field, Arc Flash |
| Overload Titan | Grid | 90 | 14 | 10 | Power Surge, Capacitor, Overvoltage |

### Boss Monsters

| Name | Track | HP | ATK | DEF | Cards |
|------|------|---:|----:|----:|-------|
| NEXUS Core | AI | 200 | 20 | 15 | Targeting Matrix, Stack Overflow, Encryption, Singularity |
| Meltdown Titan | Nuclear | 190 | 22 | 12 | Irradiate, Critical Mass, Containment Breach, Detonate |
| Storm Colossus | EcoTech | 180 | 18 | 14 | Wind Shear, Eye of Storm, Tornado, Gaia's Blessing |

## Behavior Pattern

몬스터 행동은 랜덤 선택이 아니라 순환 패턴이다.

1. 전투 시작 시 `prepareIntent()`가 호출된다.
2. `prepareIntent()`는 `intentQueue`를 비우고 `rollNextCard()` 결과를 enqueue한다.
3. 화면에는 `peekIntent()`로 다음 행동 카드 이름을 보여준다.
4. 몬스터 턴이 되면 `executeAndQueue(out)`이 큐에서 현재 행동을 dequeue한다.
5. 현재 행동을 실행한 뒤 `rollNextCard()`로 다음 행동을 enqueue한다.
6. `patternIndex`는 카드 개수 기준으로 순환한다.

예를 들어 Fission Hound의 행동 순서는 다음과 같다.

```text
Strike -> Poison Dart -> Fission Burst -> Strike -> ...
```

## Intent Queue

`Queue<Card>`는 몬스터의 다음 행동을 관리하는 자료구조다. 현재 구현에서는 큐에 항상 다음 행동 1개만 유지한다.

이 구조를 사용하는 이유는 다음과 같다.

- 플레이어에게 다음 행동을 예고할 수 있다.
- 행동 실행과 다음 행동 예약을 분리할 수 있다.
- 추후 패턴 길이를 늘리거나 특수 행동을 끼워 넣기 쉽다.

스턴 상태일 때도 `executeAndQueue(dummy)`를 호출한다. 즉, 스턴으로 행동은 취소되지만 패턴은 한 칸 진행된다.

## Battle Flow

전투에서 몬스터 행동은 `Battle::executePhase()`에서 처리된다.

1. 플레이어 캐릭터들이 먼저 배정된 카드를 실행한다.
2. 살아있는 몬스터를 순회한다.
3. 몬스터가 stun 상태면 행동을 실행하지 않고 패턴만 진행한다.
4. stun이 아니면 `executeAndQueue(card)`로 현재 행동 카드를 꺼낸다.
5. `resolveCard(card, enemy, false, -1)`로 카드 효과를 적용한다.

몬스터 카드의 효과 적용 규칙은 플레이어 카드와 동일한 `Effect` 시스템을 사용한다. 공격, 방어, 버프, 디버프, 드로우, 자리 교체 효과가 모두 같은 처리 경로를 탄다.

## Targeting Rules

몬스터가 플레이어에게 공격할 때 기본 대상은 전열 캐릭터다.

- `frontPartyIndex()`는 살아있는 첫 번째 파티원을 찾는다.
- 단일 대상 공격은 전열 1명에게 들어간다.
- 전체 대상 공격은 살아있는 모든 파티원에게 들어간다.
- 몬스터가 `Swap` 효과를 쓰면 파티 위치를 무작위로 교체한다.

## Track Match

몬스터도 `Track`을 가진다. 몬스터가 자신의 Track과 같은 Track 카드로 행동하면 플레이어와 동일하게 bonus effect가 발동된다.

예시:

- AI 몬스터가 AI 카드 사용: 기본 효과 + 보너스 효과 적용
- Nuclear 몬스터가 공용 카드 사용: 기본 효과만 적용
- EcoTech 몬스터가 EcoTech 카드 사용: 기본 효과 + 보너스 효과 적용

이 설계 덕분에 몬스터의 테마와 카드 효과가 자연스럽게 연결된다.

## Spawn Rules

몬스터 배치는 `Game::handleBattle()`에서 결정된다.

### Normal Battle

일반 전투는 다음 규칙을 따른다.

- 등장 수: 1~2마리
- `MonsterRegistry`에서 무작위 선택
- 보스 몬스터는 최대한 제외
- 보스가 뽑히면 최대 10번까지 다시 시도

### Boss Battle

보스 전투는 다음 규칙을 따른다.

- 보스 1마리만 등장
- 레지스트리를 순회하면서 `isBoss == true`인 몬스터를 찾는다.
- 현재 구현은 마지막으로 발견한 보스를 사용한다.
- 보스가 없으면 레지스트리의 마지막 몬스터를 사용한다.

현재 등록 순서 기준으로 보스 전투에서는 `Storm Colossus`가 선택될 가능성이 높다. 여러 보스 중 하나를 랜덤으로 고르려면 보스 후보 배열을 따로 만들거나 순회 중 랜덤 선택 로직을 추가해야 한다.

## Balance Notes

몬스터 밸런스는 HP, ATK, DEF, 카드 패턴의 네 축으로 조절한다.

| Axis | Low Value Effect | High Value Effect |
|------|------------------|-------------------|
| HP | 빠르게 처치 가능 | 장기전 유도 |
| ATK | 피해 압박 낮음 | 전열 캐릭터 집중 압박 |
| DEF | 공격 카드에 취약 | 약한 공격을 잘 버팀 |
| Card Pattern | 단순 예측 가능 | 상태이상/방어/광역 공격으로 복합 압박 |

일반 몬스터는 보통 카드 3장, 보스는 카드 4장으로 구성되어 있다. 일반 몬스터는 하나의 전술을 선명하게 보여주고, 보스는 공격, 방어, 상태이상, 마무리 기술을 섞어 긴장감을 만든다.

## Add New Monster

새 몬스터 추가 절차는 다음과 같다.

1. `game/src/registry/MonsterRegistry.cpp`의 `registerAllMonsters()`에 새 블록을 추가한다.
2. `EnemyDef def(name, hp, atk, def, track, isBoss)`를 만든다.
3. `addCardByName(def, cards, cardName)`으로 행동 카드를 추가한다.
4. `monsters.registerMonster(def)`를 호출한다.
5. 사용하는 카드가 `CardRegistry`에 등록되어 있는지 확인한다.

예시:

```cpp
{
    EnemyDef def("Example Sentinel", 85, 10, 8, Track::Grid);
    addCardByName(def, cards, "Strike");
    addCardByName(def, cards, "Static Field");
    addCardByName(def, cards, "Guard");
    monsters.registerMonster(def);
}
```

## Design Risks

| Risk | Cause | Mitigation |
|------|-------|------------|
| 카드 이름 불일치 | `CardRegistry`에 없는 이름을 사용 | missing card 로그 확인, 카드 등록 순서 점검 |
| 보스 선택 고정 | 현재 보스 순회 결과 마지막 보스가 선택됨 | 보스 후보를 모아 랜덤 선택 |
| 일반 전투에 보스 등장 가능성 | 보스 제외 재시도 10회 이후에도 보스일 수 있음 | 일반 몬스터 후보만 별도 필터링 |
| 빈 행동 패턴 | 카드 등록 실패 또는 cardCount 0 | `Enemy` 생성자에서 기본 Attack 카드 자동 추가 |
| 패턴 예측이 너무 쉬움 | 순환 패턴만 사용 | 조건부 카드 삽입, HP 단계별 패턴, 랜덤 분기 추가 |

## Future Extensions

- 보스 후보 중 랜덤 선택
- 방 깊이에 따른 몬스터 풀 분리
- 몬스터별 등장 가중치
- HP 50% 이하 페이즈 전환
- 특정 턴마다 강력한 필살기 삽입
- 플레이어 Track 조합에 반응하는 카운터 몬스터
- 전투 시작 시 몬스터 intent를 2~3개까지 미리 공개하는 고난도 모드

