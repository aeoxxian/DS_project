#include "ds/Sorting.h"
#include "run/Item.h"
#include "ds/ScoreTree.h"

void sortItemsByValueDescending(Item* items, int count) {
    for (int i = 0; i < count - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < count; ++j)
            if (items[j].getValue() > items[maxIdx].getValue())
                maxIdx = j;
        if (maxIdx != i) {
            Item tmp = items[i]; items[i] = items[maxIdx]; items[maxIdx] = tmp;
        }
    }
}

void sortScoresDescending(ScoreRecord* records, int count) {
    for (int i = 1; i < count; ++i) {
        ScoreRecord key = records[i];
        int j = i - 1;
        while (j >= 0 && records[j].score < key.score) {
            records[j + 1] = records[j]; --j;
        }
        records[j + 1] = key;
    }
}

void sortBattleStatsByDamage(BattleStats* stats, int count) {
    for (int i = 1; i < count; ++i) {
        BattleStats key = stats[i];
        int j = i - 1;
        while (j >= 0 && stats[j].damageDealt < key.damageDealt) {
            stats[j + 1] = stats[j]; --j;
        }
        stats[j + 1] = key;
    }
}
