#ifndef ROOM_H
#define ROOM_H

#include "Item.h"
#include "Monster.h"
#include "ds/DynamicArray.h"
#include <string>

class Room {
private:
    int id;
    std::string name;
    std::string description;
    DynamicArray<Item> items;
    DynamicArray<Monster> monsters;
    bool visited;

public:
    Room();
    Room(int id, const std::string& name, const std::string& description);

    int getId() const;
    std::string getName() const;
    std::string getDescription() const;

    void setVisited(bool value);
    bool hasBeenVisited() const;

    void addItem(const Item& item);
    bool takeItem(const std::string& itemName, Item& output);
    int itemCount() const;
    const Item& getItem(int index) const;

    void addMonster(const Monster& monster);
    int monsterCount() const;
    const Monster& getMonster(int index) const;

    void printDescription() const;
};

#endif
