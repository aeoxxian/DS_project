CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Iinclude

CORE_SRC = \
	src/core/Direction.cpp

EFFECT_SRC = \
	src/effect/Effect.cpp \
	src/effect/StatusEffect.cpp

CARD_SRC = \
	src/card/Card.cpp \
	src/card/CardPool.cpp

COMBATANT_SRC = \
	src/combatant/Combatant.cpp \
	src/combatant/BattleCharacter.cpp \
	src/combatant/Enemy.cpp

BATTLE_SRC = \
	src/battle/Battle.cpp

EVENT_SRC = \
	src/event/Event.cpp

MAP_SRC = \
	src/map/RunMap.cpp \
	src/map/Room.cpp \
	src/map/RestRoom.cpp \
	src/map/DungeonGraph.cpp

RUN_SRC = \
	src/run/Game.cpp \
	src/run/Item.cpp \
	src/run/Inventory.cpp

REGISTRY_SRC = \
	src/registry/CardRegistry.cpp \
	src/registry/CharacterRoster.cpp \
	src/registry/MonsterRegistry.cpp \
	src/registry/EventRegistry.cpp

DS_SRC = \
	src/ds/ScoreTree.cpp \
	src/ds/Sorting.cpp

SAVE_SRC = \
	src/save/UnlockSave.cpp \
	src/save/DeckSave.cpp

APP_SOURCES = src/main.cpp \
	$(CORE_SRC) $(EFFECT_SRC) $(CARD_SRC) $(COMBATANT_SRC) \
	$(BATTLE_SRC) $(EVENT_SRC) $(MAP_SRC) \
	$(RUN_SRC) $(REGISTRY_SRC) $(DS_SRC) $(SAVE_SRC)

all: dungeon_explorer

dungeon_explorer: $(APP_SOURCES)
	$(CXX) $(CXXFLAGS) $(APP_SOURCES) -o dungeon_explorer

clean:
	rm -f dungeon_explorer
