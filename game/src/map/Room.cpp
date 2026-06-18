#include "map/Room.h"
#include <iostream>

Room::Room() : id(-1), name(""), description(""), visited(false) {}

Room::Room(int id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description), visited(false) {}

int         Room::getId()          const { return id; }
std::string Room::getName()        const { return name; }
std::string Room::getDescription() const { return description; }
void        Room::setVisited(bool value) { visited = value; }
bool        Room::hasBeenVisited() const { return visited; }

void Room::addItem(const Item& item)     { items.pushBack(item); }
int  Room::itemCount()             const { return items.size(); }
const Item& Room::getItem(int index) const { return items[index]; }

bool Room::takeItem(const std::string& itemName, Item& output) {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].getName() == itemName) {
            output = items[i];
            items.removeAt(i);
            return true;
        }
    }
    return false;
}

void         Room::addEnemy(const Enemy& enemy) { monsters.pushBack(enemy); }
int          Room::enemyCount()           const { return monsters.size(); }
const Enemy& Room::getEnemy(int index)    const { return monsters[index]; }
void         Room::clearEnemies()               { monsters.clear(); }

void Room::printDescription() const {
    std::cout << "\n== " << name << " ==\n" << description << "\n";
    if (items.isEmpty()) {
        std::cout << "아이템: 없음\n";
    } else {
        std::cout << "아이템:\n";
        for (int i = 0; i < items.size(); ++i) {
            std::cout << "  - "; items[i].print();
        }
    }
    if (!monsters.isEmpty()) {
        std::cout << "적:\n";
        for (int i = 0; i < monsters.size(); ++i) {
            std::cout << "  - "; monsters[i].printStatus();
        }
    }
}
