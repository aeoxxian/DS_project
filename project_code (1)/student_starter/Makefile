CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude

# ── 기존 던전 소스 ─────────────────────────────────────────────────────────────
DUNGEON_SRC = \
	src/Direction.cpp \
	src/Player.cpp \
	src/Room.cpp \
	src/Item.cpp \
	src/Monster.cpp \
	src/Inventory.cpp \
	src/DungeonGraph.cpp \
	src/ScoreTree.cpp \
	src/Sorting.cpp

# ── 배틀/카드 시스템 ──────────────────────────────────────────────────────────
BATTLE_SRC = \
	src/Track.cpp \
	src/CardEffect.cpp \
	src/Card.cpp \
	src/CardPool.cpp \
	src/Hand.cpp \
	src/StatusEffect.cpp \
	src/BattleCharacter.cpp \
	src/Enemy.cpp \
	src/Battle.cpp

# ── 레지스트리 ────────────────────────────────────────────────────────────────
REGISTRY_SRC = \
	src/CardRegistry.cpp \
	src/CharacterRoster.cpp \
	src/MonsterRegistry.cpp \
	src/EventRegistry.cpp

# ── 이벤트 / 맵 / 런 ─────────────────────────────────────────────────────────
RUN_SRC = \
	src/Event.cpp \
	src/RunMap.cpp \
	src/RestRoom.cpp \
	src/Run.cpp

APP_SOURCES  = src/main.cpp src/Game.cpp $(DUNGEON_SRC) $(BATTLE_SRC) $(REGISTRY_SRC) $(RUN_SRC)
TEST_SOURCES = tests/ds_smoke_tests.cpp $(DUNGEON_SRC)

all: dungeon_explorer

dungeon_explorer: $(APP_SOURCES)
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o dungeon_explorer

ds_smoke_tests: $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) $(TEST_SOURCES) -o ds_smoke_tests

test: ds_smoke_tests
	./ds_smoke_tests

clean:
	rm -f dungeon_explorer ds_smoke_tests
