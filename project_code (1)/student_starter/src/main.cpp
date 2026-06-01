#include "ds/CardRegistry.h"
#include "ds/CharacterRoster.h"
#include "ds/MonsterRegistry.h"
#include "ds/EventRegistry.h"
#include "Run.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // 게임 시작 시 모든 콘텐츠 등록
    registerAllCards();
    registerAllCharacters();
    registerAllMonsters();
    registerAllEvents();

    // 런 시작
    Run run;
    run.start();

    return 0;
}
