#ifndef ROOM_H
#define ROOM_H

#include "run/Item.h"
#include "combatant/Enemy.h"
#include "combatant/BattleCharacter.h"
#include "card/CardPool.h"
#include "ds/DynamicArray.h"
#include <string>

class Room {
private:
    int id;
    std::string name;
    std::string description;
    DynamicArray<Item>  items;
    DynamicArray<Enemy> monsters;
    bool visited;

public:
    Room();
    Room(int id, const std::string& name, const std::string& description);

    int         getId()          const;
    std::string getName()        const;
    std::string getDescription() const;
    void        setVisited(bool value);
    bool        hasBeenVisited() const;

    void         addItem(const Item& item);
    bool         takeItem(const std::string& itemName, Item& output);
    int          itemCount() const;
    const Item&  getItem(int index) const;

    void         addEnemy(const Enemy& enemy);
    int          enemyCount() const;
    const Enemy& getEnemy(int index) const;
    void         clearEnemies();

    void printDescription() const;
};

// ── RestRoom ──────────────────────────────────────────────────────────────────
class RestRoom {
public:
    void enter(BattleCharacter characters[], int count, CardPool& pool);
private:
    void healOption(BattleCharacter characters[], int count);
};

#endif
