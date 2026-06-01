#include "Player.h"
#include <iostream>

Player::Player(const std::string& name)
    : name(name), health(100), score(0), currentRoomId(0), inventory() {}

std::string Player::getName() const {
    return name;
}

int Player::getHealth() const {
    return health;
}

int Player::getScore() const {
    return score;
}

int Player::getCurrentRoomId() const {
    return currentRoomId;
}

void Player::setCurrentRoomId(int roomId) {
    currentRoomId = roomId;
}

void Player::addScore(int amount) {
    score += amount;
}

void Player::changeHealth(int delta) {
    health += delta;
    if (health > 100) {
        health = 100;
    }
    if (health < 0) {
        health = 0;
    }
}

bool Player::isAlive() const {
    return health > 0;
}

Inventory& Player::getInventory() {
    return inventory;
}

const Inventory& Player::getInventory() const {
    return inventory;
}

void Player::printStatus() const {
    std::cout << "Player: " << name << " | HP: " << health << " | Score: " << score << "\n";
}
