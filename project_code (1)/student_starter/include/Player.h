#ifndef PLAYER_H
#define PLAYER_H

#include "Item.h"
#include "ds/Inventory.h"
#include <string>

class Player {
private:
    std::string name;
    int health;
    int score;
    int currentRoomId;
    Inventory inventory;

public:
    Player(const std::string& name = "Explorer");

    std::string getName() const;
    int getHealth() const;
    int getScore() const;
    int getCurrentRoomId() const;

    void setCurrentRoomId(int roomId);
    void addScore(int amount);
    void changeHealth(int delta);
    bool isAlive() const;

    Inventory& getInventory();
    const Inventory& getInventory() const;

    void printStatus() const;
};

#endif
