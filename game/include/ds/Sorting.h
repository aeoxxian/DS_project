#ifndef SORTING_H
#define SORTING_H

#include "battle/BattleStats.h"

// forward declarations — 순환 include 방지
class Item;
struct ScoreRecord;

void sortItemsByValueDescending(Item* items, int count);
void sortScoresDescending(ScoreRecord* records, int count);
void sortBattleStatsByDamage(BattleStats* stats, int count);

#endif
