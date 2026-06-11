#ifndef UNLOCK_SAVE_H
#define UNLOCK_SAVE_H

#include <string>

// 해금된 카드 이름 목록을 파일에 저장/로드
class UnlockSave {
    static const int MAX_UNLOCKS = 256;
    std::string names[MAX_UNLOCKS];
    int         count;
    std::string filepath;

public:
    explicit UnlockSave(const std::string& path = "save/unlocks.dat");

    bool load();
    void save() const;

    // name이 새로 해금됐으면 true 반환 (이미 해금됐으면 false)
    bool unlock(const std::string& name);
    bool isUnlocked(const std::string& name) const;

    int  size()    const { return count; }
    bool isEmpty() const { return count == 0; }
    const std::string& getAt(int i) const { return names[i]; }
};

#endif
