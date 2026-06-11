#ifndef DECK_SAVE_H
#define DECK_SAVE_H

#include "ds/DynamicArray.h"
#include <string>

struct DeckPreset {
    static const int MAX_CARDS = 25;
    static const int MAX_CHARS = 3;
    std::string name;
    std::string charNames[MAX_CHARS];
    int charCount;
    std::string cardNames[MAX_CARDS];
    int cardCount;

    DeckPreset() : charCount(0), cardCount(0) {}
    explicit DeckPreset(const std::string& n) : name(n), charCount(0), cardCount(0) {}

    bool addChar(const std::string& c) {
        if (charCount >= MAX_CHARS) return false;
        charNames[charCount++] = c; return true;
    }
    bool addCard(const std::string& c) {
        if (cardCount >= MAX_CARDS) return false;
        cardNames[cardCount++] = c; return true;
    }
};

// 덱 프리셋 목록을 파일에 저장/로드 (슬롯은 DynamicArray로 관리)
class DeckSave {
    DynamicArray<DeckPreset> slots;
    std::string filepath;

public:
    explicit DeckSave(const std::string& path = "save/decks.dat");

    bool load();
    void save() const;

    bool addPreset(const DeckPreset& p);
    bool removeAt(int i);

    int  size()    const;
    bool isEmpty() const;
    const DeckPreset& getAt(int i) const;

    void print() const;
};

#endif
