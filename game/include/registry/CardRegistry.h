#ifndef CARD_REGISTRY_H
#define CARD_REGISTRY_H

#include "card/Card.h"
#include "core/Constants.h"
#include <string>

class CardRegistry {
private:
    Card entries[MAX_REGISTRY_SIZE];
    int  count;
    CardRegistry();

public:
    static CardRegistry& instance();
    bool registerCard(const Card& card);
    bool findById(int id, Card& out) const;
    bool findByName(const std::string& name, Card& out) const;
    bool getAt(int index, Card& out) const;
    int  size() const;
    void printAll() const;
};

void registerAllCards();

#endif
