#include "save/DeckSave.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

DeckSave::DeckSave(const std::string& path) : filepath(path) {}

bool DeckSave::load() {
    std::ifstream f(filepath);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.size() >= 5 && line.substr(0, 5) == "DECK ") {
            DeckPreset p(line.substr(5));
            while (std::getline(f, line) && line != "END") {
                if (line.empty()) continue;
                if (line.size() >= 5 && line.substr(0, 5) == "CHAR ")
                    p.addChar(line.substr(5));
                else
                    p.addCard(line);
            }
            slots.pushBack(p);
        }
    }
    return slots.size() > 0;
}

void DeckSave::save() const {
    mkdir("save", 0755);
    std::ofstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "  [경고] 덱 저장 실패: " << filepath << "\n";
        return;
    }
    for (int i = 0; i < slots.size(); ++i) {
        const DeckPreset& p = slots[i];
        f << "DECK " << p.name << "\n";
        for (int j = 0; j < p.charCount; ++j)
            f << "CHAR " << p.charNames[j] << "\n";
        for (int j = 0; j < p.cardCount; ++j)
            f << p.cardNames[j] << "\n";
        f << "END\n";
    }
}

bool DeckSave::addPreset(const DeckPreset& p) {
    slots.pushBack(p);
    return true;
}

bool DeckSave::removeAt(int i) {
    return slots.removeAt(i);
}

int  DeckSave::size()    const { return slots.size(); }
bool DeckSave::isEmpty() const { return slots.isEmpty(); }

const DeckPreset& DeckSave::getAt(int i) const { return slots[i]; }

void DeckSave::print() const {
    for (int i = 0; i < slots.size(); ++i)
        std::cout << "  [" << i << "]  "
                  << slots[i].name
                  << "  (" << slots[i].cardCount << "장)\n";
}
