#ifndef SORTING_H
#define SORTING_H

#include "battle/BattleStats.h"
#include "run/Item.h"

// forward declarations — 순환 include 방지
struct ScoreRecord;

void sortItemsByValueDescending(Item* items, int count);       // 선택 정렬
void sortScoresDescending(ScoreRecord* records, int count);    // 선택 정렬
void sortBattleStatsByDamage(BattleStats* stats, int count);   // 삽입 정렬

#endif
