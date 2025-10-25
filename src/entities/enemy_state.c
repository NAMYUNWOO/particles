#include "enemy_state.h"
#include <stddef.h>

/**
 * @brief Get human-readable state name for debugging
 */
const char* GetStateName(EnemyStateFlags state) {
    switch (state) {
        case ENEMY_STATE_NONE:          return "NONE";
        case ENEMY_STATE_INVULNERABLE:  return "INVULNERABLE";
        case ENEMY_STATE_SHIELDED:      return "SHIELDED";
        case ENEMY_STATE_PULSED:        return "PULSED";
        case ENEMY_STATE_TELEPORTING:   return "TELEPORTING";
        case ENEMY_STATE_STORM_ACTIVE:  return "STORM_ACTIVE";
        case ENEMY_STATE_SPLITTING:     return "SPLITTING";
        case ENEMY_STATE_PHASE_CHANGE:  return "PHASE_CHANGE";
        default:                        return "UNKNOWN";
    }
}
