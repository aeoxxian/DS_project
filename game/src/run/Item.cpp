#include "run/Item.h"
#include <iostream>

Item::Item() : value(0), type(PotionType::Heal) {}
Item::Item(const std::string& name, const std::string& description,
           int value, PotionType type)
    : name(name), description(description), value(value), type(type) {}

std::string Item::getName()        const { return name; }
std::string Item::getDescription() const { return description; }
int         Item::getValue()       const { return value; }
PotionType  Item::getType()        const { return type; }

void Item::print() const {
    std::cout << name << " (value:" << value << ") — " << description << "\n";
}
