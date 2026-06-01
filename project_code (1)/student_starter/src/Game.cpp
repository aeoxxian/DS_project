#include "Game.h"
#include "ds/Sorting.h"
#include <iostream>
#include <sstream>

Game::Game() : dungeon(), player("Explorer"), moveHistory(), eventQueue(), scoreTree(), running(true) {
    buildSampleWorld();
    seedScores();
}

void Game::buildSampleWorld() {
    int entrance = dungeon.addRoom("Entrance Hall", "A cold stone hall lit by a blue torch.");
    int library = dungeon.addRoom("Library", "Dusty books cover the walls. A ladder leads to a dark balcony.");
    int armory = dungeon.addRoom("Armory", "Broken shields and old spears are scattered across the floor.");
    int vault = dungeon.addRoom("Crystal Vault", "A quiet chamber glows with light from a suspended crystal.");

    Room* entranceRoom = dungeon.getRoom(entrance);
    Room* libraryRoom = dungeon.getRoom(library);
    Room* armoryRoom = dungeon.getRoom(armory);
    Room* vaultRoom = dungeon.getRoom(vault);

    if (entranceRoom != nullptr) {
        entranceRoom->addItem(Item("key", "A small iron key.", 10));
    }
    if (libraryRoom != nullptr) {
        libraryRoom->addItem(Item("scroll", "A scroll containing a forgotten path hint.", 25));
    }
    if (armoryRoom != nullptr) {
        armoryRoom->addItem(Item("spear", "A rusty spear. It is still sharp enough.", 20));
        armoryRoom->addMonster(Monster("training dummy", 10, 0, 5));
    }
    if (vaultRoom != nullptr) {
        vaultRoom->addItem(Item("crystal", "A glowing crystal worth many points.", 50));
    }

    dungeon.connectRooms(entrance, Direction::East, library, true);
    dungeon.connectRooms(entrance, Direction::South, armory, true);
    dungeon.connectRooms(library, Direction::South, vault, true);
    dungeon.connectRooms(armory, Direction::East, vault, true);

    eventQueue.enqueue(GameEvent("A cold wind passes through the dungeon.", 0, 0));
    eventQueue.enqueue(GameEvent("You notice old markings and gain insight.", 5, 0));
    eventQueue.enqueue(GameEvent("A loose stone falls near you.", 0, -5));
}

void Game::seedScores() {
    scoreTree.insert(ScoreRecord("Ada", 80));
    scoreTree.insert(ScoreRecord("Grace", 95));
    scoreTree.insert(ScoreRecord("Linus", 60));
}

void Game::printHelp() const {
    std::cout << "\nCommands:\n";
    std::cout << "  help             Show this help message\n";
    std::cout << "  look             Describe the current room\n";
    std::cout << "  move <dir>       Move north, south, east, or west\n";
    std::cout << "  take <item>      Pick up an item in the current room\n";
    std::cout << "  inventory        Show inventory\n";
    std::cout << "  undo             Undo the previous move using a stack\n";
    std::cout << "  event            Process one queued event\n";
    std::cout << "  scores           Show score tree\n";
    std::cout << "  sortitems        Show current room items sorted by value\n";
    std::cout << "  map              Show dungeon graph\n";
    std::cout << "  status           Show player status\n";
    std::cout << "  quit             Exit the game\n";
}

void Game::look() const {
    const Room* room = dungeon.getRoom(player.getCurrentRoomId());
    if (room == nullptr) {
        std::cout << "Current room is invalid.\n";
        return;
    }

    room->printDescription();
    std::cout << "Exits:";
    bool hasExit = false;
    for (int i = 0; i < 4; ++i) {
        Direction direction = static_cast<Direction>(i);
        int neighbor = dungeon.getNeighbor(room->getId(), direction);
        if (neighbor != -1) {
            std::cout << " " << directionToString(direction);
            hasExit = true;
        }
    }
    if (!hasExit) {
        std::cout << " none or graph not implemented yet";
    }
    std::cout << "\n";
}

void Game::move(Direction direction) {
    if (direction == Direction::Invalid) {
        std::cout << "Invalid direction. Try north, south, east, or west.\n";
        return;
    }

    int current = player.getCurrentRoomId();
    int next = dungeon.getNeighbor(current, direction);
    if (next == -1) {
        std::cout << "You cannot move " << directionToString(direction) << " from here.\n";
        return;
    }

    moveHistory.push(current);
    player.setCurrentRoomId(next);
    player.addScore(1);
    look();
}

void Game::undoMove() {
    int previousRoom = -1;
    if (!moveHistory.pop(previousRoom)) {
        std::cout << "No previous move to undo.\n";
        return;
    }

    player.setCurrentRoomId(previousRoom);
    std::cout << "You step back to the previous room.\n";
    look();
}

void Game::takeItem(const std::string& itemName) {
    if (itemName.empty()) {
        std::cout << "Take what? Example: take key\n";
        return;
    }

    Room* room = dungeon.getRoom(player.getCurrentRoomId());
    if (room == nullptr) {
        std::cout << "Current room is invalid.\n";
        return;
    }

    Item item;
    if (!room->takeItem(itemName, item)) {
        std::cout << "There is no item named '" << itemName << "' here.\n";
        return;
    }

    player.getInventory().addItem(item);
    player.addScore(item.getValue());
    std::cout << "You picked up: " << item.getName() << "\n";
}

void Game::processOneEvent() {
    GameEvent event;
    if (!eventQueue.dequeue(event)) {
        std::cout << "No queued events.\n";
        return;
    }

    std::cout << "Event: " << event.description << "\n";
    player.addScore(event.scoreDelta);
    player.changeHealth(event.healthDelta);
    player.printStatus();
}

void Game::showScores() const {
    std::cout << "Score records:\n";
    scoreTree.printDescending();
}

void Game::showSortedRoomItems() const {
    const Room* room = dungeon.getRoom(player.getCurrentRoomId());
    if (room == nullptr) {
        std::cout << "Current room is invalid.\n";
        return;
    }

    int count = room->itemCount();
    if (count == 0) {
        std::cout << "There are no items to sort in this room.\n";
        return;
    }

    Item* copy = new Item[count];
    for (int i = 0; i < count; ++i) {
        copy[i] = room->getItem(i);
    }

    sortItemsByValueDescending(copy, count);

    std::cout << "Items sorted by value descending:\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  - ";
        copy[i].print();
    }

    delete[] copy;
}

void Game::processCommand(const std::string& line) {
    std::istringstream input(line);
    std::string command;
    input >> command;

    if (command == "help") {
        printHelp();
    } else if (command == "look") {
        look();
    } else if (command == "move") {
        std::string directionText;
        input >> directionText;
        move(parseDirection(directionText));
    } else if (command == "take") {
        std::string itemName;
        input >> itemName;
        takeItem(itemName);
    } else if (command == "inventory") {
        player.getInventory().print();
    } else if (command == "undo") {
        undoMove();
    } else if (command == "event") {
        processOneEvent();
    } else if (command == "scores") {
        showScores();
    } else if (command == "sortitems") {
        showSortedRoomItems();
    } else if (command == "map") {
        dungeon.printMap();
    } else if (command == "status") {
        player.printStatus();
    } else if (command == "quit") {
        running = false;
    } else if (command.empty()) {
        // Ignore blank lines.
    } else {
        std::cout << "Unknown command. Type 'help' for options.\n";
    }
}

void Game::run() {
    std::cout << "Dungeon Explorer: Guided Data Structures PBL\n";
    std::cout << "Type 'help' for commands.\n";
    look();

    while (running && player.isAlive()) {
        std::cout << "\n> ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;
        }
        processCommand(line);
    }

    scoreTree.insert(ScoreRecord(player.getName(), player.getScore()));
    std::cout << "\nFinal status:\n";
    player.printStatus();
    std::cout << "Goodbye.\n";
}
