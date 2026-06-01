#ifndef CONSTANTS_H
#define CONSTANTS_H

// ── Battle ────────────────────────────────────────────────────────────────────
static const int MAX_CHARACTERS     = 3;
static const int MAX_ENEMIES        = 4;
static const int TRACKS_PER_CHARACTER = 1;
static const int TOTAL_TRACKS       = 6;
static const int DRAW_PER_TURN      = 5;
static const int BATTLE_REWARD_COUNT = 3;

// ── Hand / Pool ───────────────────────────────────────────────────────────────
static const int MAX_HAND_SIZE      = 10;
static const int MAX_POOL_SIZE      = 64;

// ── Card ──────────────────────────────────────────────────────────────────────
static const int MAX_EFFECTS_PER_CARD = 4;

// ── Registries ────────────────────────────────────────────────────────────────
static const int MAX_REGISTRY_SIZE         = 256;
static const int MAX_ROSTER_SIZE           = 32;
static const int MAX_MONSTER_REGISTRY_SIZE = 128;

// ── Event ─────────────────────────────────────────────────────────────────────
static const int MAX_CHOICES        = 6;  // 이벤트당 최대 선택지 수
static const int MAX_EVENT_REGISTRY = 64;

// ── Status effects ────────────────────────────────────────────────────────────
static const int MAX_STATUS_EFFECTS = 8;

#endif
