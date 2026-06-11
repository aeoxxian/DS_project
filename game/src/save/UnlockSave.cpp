#include "save/UnlockSave.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

UnlockSave::UnlockSave(const std::string& path) : count(0), filepath(path) {}

bool UnlockSave::load() {
    std::ifstream f(filepath);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && count < MAX_UNLOCKS)
            names[count++] = line;
    }
    return count > 0;
}

void UnlockSave::save() const {
    mkdir("save", 0755);  // 없으면 생성, 있으면 무시
    std::ofstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "  [경고] 저장 실패: " << filepath << "\n";
        return;
    }
    for (int i = 0; i < count; ++i)
        f << names[i] << "\n";
}

bool UnlockSave::unlock(const std::string& name) {
    if (isUnlocked(name) || count >= MAX_UNLOCKS) return false;
    names[count++] = name;
    return true;
}

bool UnlockSave::isUnlocked(const std::string& name) const {
    for (int i = 0; i < count; ++i)
        if (names[i] == name) return true;
    return false;
}
