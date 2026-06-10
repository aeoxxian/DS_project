#ifndef ITEM_H
#define ITEM_H

#include <string>

enum class PotionType { Heal, AtkUp, Shield, Poison, Explosive };

class Item {
private:
    std::string name;
    std::string description;
    int         value;
    PotionType  type;

public:
    Item();
    Item(const std::string& name, const std::string& description,
         int value, PotionType type = PotionType::Heal);

    std::string getName()        const;
    std::string getDescription() const;
    int         getValue()       const;
    PotionType  getType()        const;

    void print() const;
};

#endif
