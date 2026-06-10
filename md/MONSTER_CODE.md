# Monster Code

## Purpose

This document explains the monster-side combat code after the refactor.

The main goal of the refactor is simple:

- Players use `Card`.
- Monsters also use `Card`.
- Both sides resolve cards through the same `Battle` logic.
- Monster behavior is controlled by a repeating card pattern.

Before the refactor, monsters used an `EnemySkill` wrapper. That wrapper only
contained a `Card`, so it was removed. Monsters now store `Card` objects
directly.

## Main Files

| File | Description |
| --- | --- |
| `include/registry/MonsterRegistry.h` | Defines `EnemyDef`, including monster stats and card list. |
| `src/registry/MonsterRegistry.cpp` | Registers monsters and assigns cards from `CardRegistry`. |
| `include/combatant/Enemy.h` | Defines the runtime enemy card pattern and intent queue. |
| `src/combatant/Enemy.cpp` | Implements monster card selection. |
| `include/battle/Battle.h` | Declares shared card resolution helpers. |
| `src/battle/Battle.cpp` | Resolves both player and monster cards. |

## EnemyDef

`EnemyDef` is the registry-time definition of a monster.

It now stores cards directly:

```cpp
struct EnemyDef : public CombatantDef {
    bool isBoss;
    Card cards[MAX_ENEMY_SKILLS];
    int  cardCount;

    bool addCard(const Card& c) {
        if (cardCount >= MAX_ENEMY_SKILLS) return false;
        cards[cardCount++] = c;
        return true;
    }
};
```

This replaces the old `EnemySkill skills[]` structure.

## Enemy Runtime Pattern

`Enemy` copies the cards from `EnemyDef` and chooses them in order.

Example:

```text
Strike -> Poison Dart -> Fission Burst -> Strike -> ...
```

The core method is:

```cpp
Card Enemy::rollNextCard() {
    if (cardCount == 0) return Card();
    Card next = cards[patternIndex % cardCount];
    patternIndex = (patternIndex + 1) % cardCount;
    return next;
}
```

## Intent Queue

Each enemy keeps a one-card intent queue.

```cpp
void Enemy::prepareIntent() {
    intentQueue.clear();
    intentQueue.enqueue(rollNextCard());
}
```

When the enemy acts, it dequeues the current card and queues the next one:

```cpp
bool Enemy::executeAndQueue(Card& out) {
    if (!intentQueue.dequeue(out)) return false;
    intentQueue.enqueue(rollNextCard());
    return true;
}
```

This lets the battle screen show the monster's next action:

```cpp
Card intent;
std::string intentName = e.peekIntent(intent) ? intent.getName() : "???";
```

## MonsterRegistry

Monsters reuse existing cards from `CardRegistry`.

Helper:

```cpp
static void addCardByName(EnemyDef& def, CardRegistry& cards,
                          const std::string& name) {
    Card card;
    if (cards.findByName(name, card)) {
        def.addCard(card);
    } else {
        std::cout << "[MonsterRegistry] missing card: " << name << "\n";
    }
}
```

Example:

```cpp
EnemyDef def("Fission Hound", 60, 8, 4, Track::Nuclear);
addCardByName(def, cards, "Strike");
addCardByName(def, cards, "Poison Dart");
addCardByName(def, cards, "Fission Burst");
monsters.registerMonster(def);
```

This means balancing a monster can be done by changing its card list.

## Registration Order

Cards must be registered before monsters.

The current order is correct:

```cpp
registerAllCards();
registerAllCharacters();
registerAllMonsters();
registerAllEvents();
```

## Shared Card Resolution

`Battle::resolveCard()` is the shared path for both player and monster cards.

```cpp
void Battle::resolveCard(const Card& card, Combatant& source,
                         bool sourceIsPlayer, int targetIdx) {
    for (int ef = 0; ef < card.getEffectCount(); ++ef)
        applyEffect(card.getEffect(ef), source, targetIdx, sourceIsPlayer);

    if (card.isTrackCard() && source.hasTrack(card.getTrack())) {
        for (int ef = 0; ef < card.getBonusEffectCount(); ++ef)
            applyEffect(card.getBonusEffect(ef), source, targetIdx, sourceIsPlayer);
    }
}
```

Player use:

```cpp
resolveCard(card, *party[ci], true, firstLivingEnemy());
```

Monster use:

```cpp
Card card;
if (!enemies[ei].executeAndQueue(card)) continue;
resolveCard(card, enemies[ei], false, -1);
```

## Target Rules

The same card can mean slightly different targets depending on who uses it.

| Effect target | Player source | Monster source |
| --- | --- | --- |
| Default attack | Selected/front enemy | Front party member |
| `AllEnemies` | All monsters | All party members |
| `Party` buff/defense | Player party | Monster side |

This keeps monster behavior compatible with existing player cards.

## Battle Flow

```text
1. Battle starts.
2. Each enemy prepares intent.
3. Player assigns cards.
4. Player cards resolve through Battle::resolveCard().
5. Monster cards resolve through Battle::resolveCard().
6. Monsters queue their next pattern card.
7. Status effects tick.
8. Next turn begins.
```

## Benefits

- No separate monster-only card system
- Less duplicated combat code
- Existing cards can be reused for monsters
- Track bonus effects can also work for monsters
- Monster patterns are easy to read and balance
- Future card changes affect both player and monster logic consistently

