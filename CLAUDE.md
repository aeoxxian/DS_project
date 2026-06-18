# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A university **Data Structures PBL** assignment: *Dungeon Explorer (지식의 상아탑)*, a terminal-based, text roguelike card-battle game written in C++17. The grade depends on each required data structure being **hand-implemented** (no STL containers) and **meaningfully wired into gameplay** — not just present. The grading rubric is `docs/RUBRIC.md`; live status against it is tracked in `md/PROGRESS.md`. Source comments and all in-game text are Korean (UTF-8); preserve this.

## Two code trees — only `game/` is canonical

- **`game/`** — the current, maintained implementation. Modular `include/` + `src/` split by subsystem, built with its `Makefile`. **Do all work here.**
- **`project_code (1)/student_starter/`** — an older, divergent variant with a flat layout (`Run` instead of `Game`, all headers in one dir) and a stale prebuilt `.exe`. Do not edit unless explicitly asked; changes here do not affect the real game.

## Build & run

```bash
cd game
make            # g++ -std=c++17 -Wall -Wextra -pedantic, outputs ./dungeon_explorer
./dungeon_explorer
make clean
```

There is no test suite; "smoke test" in the docs means a clean `make` with no warnings, plus manual playthrough. The Makefile lists every `.cpp` explicitly (no wildcard) — **add new `src/*.cpp` files to the correct `*_SRC` variable in `game/Makefile` or they won't link.**

## Hard constraint: no STL containers

`vector`, `list`, `stack`, `queue`, `map`, etc. are **forbidden** — the whole point of the assignment is the custom implementations in `include/ds/`. When you need a container, use these (all header-only templates except where noted):

| Need | Use | Backs |
|------|-----|-------|
| Sequence / shared deck | `ds/LinkedList.h` | CardPool, Hand, Inventory |
| LIFO / undo / DFS | `ds/Stack.h` | RunMap move-undo, Battle assign-undo, DungeonGraph DFS |
| FIFO / cyclic intent | `ds/Queue.h` | `Enemy` intent pattern (`Queue<Card>`) |
| Indexed list | `ds/DynamicArray.h` | items/enemies inside a `Room`; deck-preset slots in `DeckSave` |
| Ranking | `ds/ScoreTree.h` (+ `src/ds/ScoreTree.cpp`) | battle-efficiency BST, duplicates inserted right |
| Sorting | `ds/Sorting.h` (+ `src/ds/Sorting.cpp`) | run-end stats (selection + insertion sort) |
| Room graph | `map/DungeonGraph.h` (+ `.cpp`) | 4-direction adjacency, DFS |

## Architecture

Flow: `main.cpp` → `registerAll()` (loads all content into global registries) → title screen → `Game::run()`. `main` wraps the run in a retry loop (`do { Game game(retry); retry = game.run(); } while (retry)`), so `Game(bool isRetry)` and the `bool` return of `run()` carry the "play again" decision between runs.

- **Registry-driven content.** All game content is declared in `src/registry/` and loaded once at startup via `registerAll()` (`include/registry/Registries.h`): cards (`CardRegistry`), playable characters (`CharacterRoster`), monsters (`MonsterRegistry`), events (`EventRegistry`). **To add/tune content, edit the registry `.cpp` — not the engine classes.** `md/README.md` documents the exact factory-call patterns (`Effects::attack(...)`, `addCardByName(...)`, `EventChoice::addOutcome(...)`, etc.).

- **`Game` (`run/Game.h`)** owns a run: a `party[]` of `BattleCharacter`, the shared `CardPool`, the `RunMap`, `Inventory`, and the `ScoreTree`/`BattleStats` log. It walks the map and dispatches each room to `handleBattle` / `handleEvent` / `handleShop` / `handleRest`, then `printRunSummary()` at the end. (The gold system was removed — `handleShop` is a free-supply event and the `GainGold`/`LoseGold` outcomes are now no-ops; don't reintroduce a `gold` member.)

- **Persistence (`save/`).** Two cross-run save files live in `game/save/`. `UnlockSave` (`save/unlocks.dat`) tracks which cards the player has ever unlocked — newly drafted cards persist so they can be picked from an unlocked-card deck on later runs. `DeckSave` (`save/decks.dat`) stores named party+deck presets in a `DynamicArray<DeckPreset>`. Both are owned by `Game`, loaded at construction, and surfaced through `showTitleScreen()` (default deck / unlocked deck / load preset). Saves are plain text keyed by card/character **name** — renaming registry content silently invalidates existing saves.

- **Combat (`battle/Battle.cpp`)** is card-assignment based: each turn draws a 5-card hand, the player assigns one card per character (or uses `switch` to swap formation positions instead of acting). Player and enemy cards run through one shared `resolveCard()`; `applyEffect()` branches friend/foe on `sourceIsPlayer`. Assignments are undoable via a `Stack<AssignRecord>`. Only the `★전열` (front-row) character takes enemy hits.

- **Effects** are data, not code: a `Card` holds `Effect` structs built by factories in `effect/Effect.h` (`namespace Effects`). Track cards carry a `bonusEffect` that fires only when the card's `Track` matches. Status effects (poison/stun/weaken/confuse) live in `effect/StatusEffect.h`.

- **Map.** `RunMap` is the tree of rooms actually traversed (with `Stack`-based move-undo). `DungeonGraph` is the separate adjacency-graph + DFS structure required by the rubric — verify it's actually reachable from the `Game` loop / a UI command (`g/graph`), since "implemented but unused" loses integration points.

- **UI (`core/UI.h`)** is a header-only namespace of box-drawing/HP-bar/typewriter helpers. `core/Constants.h` is the single source for tunable constants (`MAX_CHARACTERS`, `MAX_MAP_NODES`, etc.) — change values there, not inline.

## Conventions

- New subsystem code follows the `include/<area>/X.h` + `src/<area>/X.cpp` split, with include-guarded headers (`#ifndef`) and includes written relative to `include/` (e.g. `#include "ds/Stack.h"`), matching the `-Iinclude` flag.
- Keep the build warning-clean under `-Wall -Wextra -pedantic` — the rubric counts it.
- Korean identifiers/strings are intentional; match the surrounding language when adding user-facing text.
