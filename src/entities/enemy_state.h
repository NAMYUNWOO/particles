#ifndef ENEMY_STATE_H
#define ENEMY_STATE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Enemy state flags (boolean states only)
 *
 * Uses bitflags for efficient boolean state storage.
 * Each state occupies 1 bit, allowing 32 states in a uint32_t.
 */
typedef enum {
    ENEMY_STATE_NONE          = 0,
    ENEMY_STATE_INVULNERABLE  = 1 << 0,  // Cannot take damage
    ENEMY_STATE_SHIELDED      = 1 << 1,  // Has active shield
    ENEMY_STATE_PULSED        = 1 << 2,  // BLACKHOLE has pulsed once
    ENEMY_STATE_TELEPORTING   = 1 << 3,  // Teleporter ability active
    ENEMY_STATE_STORM_ACTIVE  = 1 << 4,  // BLACKHOLE storm cycle active
    ENEMY_STATE_SPLITTING     = 1 << 5,  // SPLITTER actively splitting
    ENEMY_STATE_PHASE_CHANGE  = 1 << 6,  // BOSS phase transition in progress
} EnemyStateFlags;

/**
 * @brief Enemy state data (numeric values)
 *
 * Stores numeric state values that can't be represented as bitflags.
 */
typedef struct {
    int phase;              // Boss phase (0, 1, 2)
    int splitCount;         // Splitter splits remaining (0~3)
    float shieldHealth;     // Shield HP (0.0~max)
    float phaseTimer;       // Phase transition timer
    float transformTimer;   // Blackhole transformation timer
    float stormCycleTimer;  // Blackhole storm cycle timer (0~10s)
} EnemyStateData;

/**
 * @brief Check if enemy has specific state flag
 * @param flags State flags to check
 * @param state State flag to test
 * @return true if state is set
 */
static inline bool HasState(uint32_t flags, EnemyStateFlags state) {
    return (flags & state) != 0;
}

/**
 * @brief Set state flag
 * @param flags Pointer to state flags
 * @param state State flag to set
 */
static inline void SetState(uint32_t* flags, EnemyStateFlags state) {
    *flags |= state;
}

/**
 * @brief Clear state flag
 * @param flags Pointer to state flags
 * @param state State flag to clear
 */
static inline void ClearState(uint32_t* flags, EnemyStateFlags state) {
    *flags &= ~state;
}

/**
 * @brief Toggle state flag
 * @param flags Pointer to state flags
 * @param state State flag to toggle
 */
static inline void ToggleState(uint32_t* flags, EnemyStateFlags state) {
    *flags ^= state;
}

/**
 * @brief Get human-readable state name for debugging
 * @param state State flag
 * @return State name string
 */
const char* GetStateName(EnemyStateFlags state);

#endif // ENEMY_STATE_H
