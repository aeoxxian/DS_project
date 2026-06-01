#ifndef ENEMY_H
#define ENEMY_H

#include "Constants.h"
#include "Track.h"
#include "StatusEffect.h"
#include <string>

enum class EnemyIntent {
    Attack,
    Defend,
    Buff,
    Unknown
};

class Enemy {
private:
    std::string name;
    int hp;
    int maxHp;
    int attackPower;
    Track track;
    EnemyIntent currentIntent;

    StatusTracker status;

public:
    Enemy();
    Enemy(const std::string& name, int maxHp, int attackPower, Track track);

    // ── Stats ─────────────────────────────────────────────────────
    std::string getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getAttackPower() const;  // returns base + atk modifier
    Track getTrack() const;
    EnemyIntent getIntent() const;
    bool isAlive() const;

    void takeDamage(int amount);

    // ── Intent ────────────────────────────────────────────────────
    void decideIntent();
    int executeIntent();         // returns damage dealt (0 if non-attack)

    // ── Status effects ────────────────────────────────────────────
    StatusTracker& getStatus();
    void tickStatus();           // called at end of turn

    // ── Display ───────────────────────────────────────────────────
    void printStatus() const;

private:
    std::string intentToString() const;
};

#endif
