#ifndef CONSTANTS_H
#define CONSTANTS_H

// ── Battle ────────────────────────────────────────────────────────────────────
static const int MAX_CHARACTERS      = 3;
static const int MAX_ENEMIES         = 4;
static const int TRACKS_PER_CHARACTER = 1;
static const int TOTAL_TRACKS        = 6;
static const int DRAW_PER_TURN       = 5;
static const int BATTLE_REWARD_COUNT = 3;

// ── Hand / Pool ───────────────────────────────────────────────────────────────
static const int MAX_HAND_SIZE       = 10;
static const int MAX_POOL_SIZE       = 64;
static const int DECK_TRACK_LIMIT    = 5;   // 트랙별 최대 카드 수
static const int DECK_COMMON_LIMIT   = 10;  // 공용 카드 최대 수

// ── Card ──────────────────────────────────────────────────────────────────────
static const int MAX_EFFECTS_PER_CARD = 4;

// ── Enemy ─────────────────────────────────────────────────────────────────────
static const int MAX_ENEMY_SKILLS = 8;

// ── Map (트리) ────────────────────────────────────────────────────────────────
static const int MAX_MAP_NODES = 48;
static const int MAP_DEPTH     = 9;
static const int MAX_CHILDREN  = 3;

// ── Map (그래프) ──────────────────────────────────────────────────────────────
static const int MAX_ROOMS     = 32;
static const int MAP_FLOORS    = 3;
static const int MAP_ROWS      = 3;
static const int MAP_COLS      = 3;

// ── Registries ────────────────────────────────────────────────────────────────
static const int MAX_REGISTRY_SIZE         = 256;
static const int MAX_ROSTER_SIZE           = 32;
static const int MAX_MONSTER_REGISTRY_SIZE = 128;
static const int MAX_EVENT_REGISTRY        = 64;

// ── Event ─────────────────────────────────────────────────────────────────────
static const int MAX_CHOICES              = 6;
static const int MAX_OUTCOMES_PER_CHOICE  = 4;

// ── Inventory ─────────────────────────────────────────────────────────────────
static const int MAX_POTIONS = 5;

// ── Status effects ────────────────────────────────────────────────────────────
static const int MAX_STATUS_EFFECTS = 24;

#endif
