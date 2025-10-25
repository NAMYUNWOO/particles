# Enemy State System Refactoring - Hybrid Approach (stateFlags + stateData)

**PRP Version**: 1.0
**Date**: 2025-10-26
**Status**: Ready for Implementation
**Confidence Score**: 9/10

---

## Goal

Refactor the **Enemy struct** to use a unified hybrid state management system combining **bitflags for boolean states** (`stateFlags`) and a **struct for numeric data** (`stateData`). This refactoring will:

1. Enable systematic state testing in test mode
2. Fix BLACKHOLE invulnerability bug in test mode
3. Provide consistent state management across all 11 enemy types
4. Improve debugging with state visualization UI
5. Simplify adding new enemy states and types

## Why

### Current Problems

**1. Scattered State Fields**: Enemy struct has 9 separate state fields scattered throughout:
- Boolean states: `hasShield`, `isInvulnerable`, `hasPulsed` (3 fields)
- Numeric states: `phase`, `splitCount`, `shieldHealth`, `phaseTimer`, `transformTimer`, `stormCycleTimer` (6 fields)

**2. Hard to Debug**: No single place to inspect enemy state - must check 9 different fields

**3. Difficult to Test**: Test mode cannot easily display or manipulate enemy states

**4. Bug-Prone**: BLACKHOLE stays invulnerable in test mode because state transitions aren't visible

**5. High Coupling**: 27 direct field accesses across codebase (24 in `enemy.c`, 3 in `enemy_manager.c`)

### Business Value

- **Faster Development**: New enemy types can reuse state system
- **Better QA**: Test mode state visualization catches bugs early
- **Easier Debugging**: Single `stateFlags` variable shows all boolean states
- **Reduced Bugs**: Centralized state management prevents inconsistent states
- **Scalability**: Adding new states requires minimal code changes

## What

### User-Visible Behavior

**Test Mode State Visualization** (New Feature):
- State flags shown as hexadecimal (e.g., `Flags: 0x05` means INVULNERABLE + PULSED)
- Active state names displayed as badges (e.g., `[INVULNERABLE] [SHIELDED]`)
- Numeric data shown when non-zero (e.g., `Phase: 2`, `Shield: 150.0`)
- Toggle states with keyboard shortcuts (I=invulnerable, S=shield, P=phase+1)

**Developer Experience**:
- Cleaner code: `HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)` instead of `enemy->isInvulnerable`
- Easier debugging: Inspect single `stateFlags` variable in debugger
- Consistent patterns: All boolean checks use `HasState()`, all sets use `SetState()`

### Technical Requirements

1. **New Files**: Create `enemy_state.h` and `enemy_state.c` with:
   - `EnemyStateFlags` enum (7 bitflags)
   - `EnemyStateData` struct (6 numeric fields)
   - Helper functions: `HasState()`, `SetState()`, `ClearState()`, `ToggleState()`
   - Debug function: `GetStateName()`

2. **Refactored Files**:
   - `enemy.h`: Add `uint32_t stateFlags` and `EnemyStateData stateData`
   - `enemy.c`: Convert 24 usages from direct field access to helper functions
   - `enemy_manager.c`: Convert 3 usages
   - `game.c`: Convert BLACKHOLE transformation logic (3 usages)
   - `dev_test_mode.c`: Add state visualization UI (+60 lines)

3. **Build System**: Update `Makefile` and `Makefile.web` with `enemy_state.c`

4. **Testing**: Unit tests for state helper functions and conversions

### Success Criteria

- [ ] All 27 state field usages converted to new system
- [ ] Desktop build compiles without warnings
- [ ] WebAssembly build compiles without warnings
- [ ] All existing enemy behaviors work identically (BLACKHOLE, BOSS, SPLITTER, etc.)
- [ ] Test mode displays enemy states in UI
- [ ] BLACKHOLE transformation visible in test mode
- [ ] Unit tests pass for state helper functions
- [ ] Enemy struct size increase < 50 bytes
- [ ] No performance regression (60 FPS with 100k particles maintained)
- [ ] Documentation updated (PLANNING.md)

---

## All Needed Context

### Documentation & References

```yaml
# MUST READ - Core Context

- file: TASK.md
  why: |
    Complete refactoring plan with all 6 phases
    Conversion patterns for 27 usage locations
    Estimated time: 60 minutes
    Risk mitigation strategies

- file: src/entities/enemy.h
  lines: 71-88
  why: |
    Current Enemy struct with scattered state fields
    9 fields to be consolidated:
      - bool hasShield (line 72)
      - int splitCount (line 71)
      - float shieldHealth (line 73)
      - int phase (line 81)
      - float phaseTimer (line 82)
      - bool isInvulnerable (line 83)
      - bool hasPulsed (line 86)
      - float transformTimer (line 87)
      - float stormCycleTimer (line 88)

- file: src/entities/enemy.c
  lines: 332-342, 548, 768-776
  why: |
    24 direct field accesses to convert
    Examples:
      - enemy->isInvulnerable = true (line 332)
      - if (enemy->isInvulnerable && enemy->phaseTimer > 1.5f) (line 341)
      - if (enemy->hasShield && enemy->shieldHealth > 0) (line 771)
      - enemy->hasShield = false (line 776)

    Critical patterns:
      - Boolean assignment: enemy->field = value
      - Boolean check: if (enemy->field)
      - Numeric assignment: enemy->field = value
      - Numeric comparison: if (enemy->field > threshold)

- file: src/entities/managers/enemy_manager.c
  why: |
    3 additional state field accesses
    Spawning and AI update logic uses enemy states

- file: src/core/game.c
  lines: 382-395
  why: |
    BLACKHOLE transformation logic (root cause of test mode bug)
    Checks: otherEnemiesCount == 0 && isInvulnerable && !hasPulsed
    Sets: hasPulsed = true, isInvulnerable = false
    This logic is invisible in current test mode

- file: src/core/dev_test_mode.c
  why: |
    Test mode rendering (DrawTestModeUI function)
    Need to add state visualization panel
    Current UI: enemy selector, spawn/remove stats
    New UI: state flags display, active state badges, numeric data

- file: tests/unit/test_dev_test_mode.c
  why: |
    Existing unit test pattern with MinUnit
    Shows test setup/teardown pattern
    Example assertions: mu_assert, mu_assert_int_eq
    Test fixture usage: test_setup/test_teardown

- url: https://en.cppreference.com/w/c/language/inline
  section: Inline Functions in C
  why: |
    Helper functions should be static inline for performance
    Inline functions in .h file are optimized away by compiler
    No function call overhead for simple bitflag operations

- docfile: PLANNING.md
  section: Enemy Types Reference (lines 117-136)
  why: |
    11 enemy types with different state requirements
    BLACKHOLE: transformation states (invulnerable → pulsed → storm)
    BOSS_1/FINAL_BOSS: phase transitions (0 → 1 → 2)
    SPLITTER: split count tracking (3 → 2 → 1 → 0)
    SHIELD enemies: shield health and active state

- docfile: RAYLIB_CONVENTIONS.md
  why: |
    Coding style for this project
    Function naming, comment style, struct conventions

- docfile: CLAUDE.md
  section: Code Structure & Modularity
  critical: |
    - Never create files > 500 lines
    - Always create unit tests for new features
    - Memory safe programming - avoid dynamic allocation
    - Update PLANNING.md when architecture changes
```

### Current Codebase Tree

```
particles/
├── src/
│   ├── core/
│   │   ├── game.c                      # BLACKHOLE transformation logic (3 usages)
│   │   └── dev_test_mode.c             # Test mode UI (needs state visualization)
│   ├── entities/
│   │   ├── enemy.h                     # Enemy struct (9 scattered fields)
│   │   ├── enemy.c                     # Main enemy logic (24 usages)
│   │   └── managers/
│   │       └── enemy_manager.c         # Enemy spawning (3 usages)
├── tests/
│   └── unit/
│       └── test_dev_test_mode.c        # MinUnit test example
├── Makefile                            # Desktop build (needs enemy_state.c)
├── Makefile.web                        # Web build (needs enemy_state.c)
└── PLANNING.md                         # Documentation (needs state system docs)
```

### Desired Codebase Tree (Files to Add)

```
particles/
├── src/
│   ├── entities/
│   │   ├── enemy_state.h               # NEW: State system interface (~80 lines)
│   │   └── enemy_state.c               # NEW: State system implementation (~50 lines)
├── tests/
│   └── unit/
│       └── test_enemy_state.c          # NEW: Unit tests for state system (~150 lines)
```

**File Responsibilities:**

- **enemy_state.h**:
  - `EnemyStateFlags` enum (7 bitflags)
  - `EnemyStateData` struct (6 numeric fields)
  - `HasState()`, `SetState()`, `ClearState()`, `ToggleState()` inline helpers
  - `GetStateName()` declaration for debugging

- **enemy_state.c**:
  - `GetStateName()` implementation
  - State validation functions (future use)

- **test_enemy_state.c**:
  - Unit tests for all helper functions
  - State transition tests
  - Bitflag manipulation tests

---

## Implementation Blueprint

### Phase 1: New State System Definition (Create New Files)

**Estimated Time**: 10 minutes

#### Step 1.1: Create State System Header

**File**: `src/entities/enemy_state.h`

```c
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
```

**Validation**:
```bash
# Check header compiles standalone
gcc -c src/entities/enemy_state.h -o /tmp/enemy_state.h.gch
# Expected: No errors
```

---

#### Step 1.2: Create State System Implementation

**File**: `src/entities/enemy_state.c`

```c
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
```

**Validation**:
```bash
# Compile together
gcc -c src/entities/enemy_state.c -o /tmp/enemy_state.o
# Expected: No errors, no warnings
```

---

### Phase 2: Enemy Struct Modification

**Estimated Time**: 5 minutes

#### Step 2.1: Update Enemy Header

**File**: `src/entities/enemy.h`

Add include at top:
```c
#include "enemy_state.h"
```

Modify Enemy struct (around line 70-90):

```c
typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    float health;
    EnemyType type;
    float speed;
    int scoreValue;
    MovePattern movePattern;
    Vector2 targetPosition;
    float stateChangeTimer;

    // NEW: Unified state system
    uint32_t stateFlags;      // Bitflags for boolean states
    EnemyStateData stateData; // Numeric state values

    // DEPRECATED (remove after refactoring complete)
    // bool hasShield;         → stateFlags & ENEMY_STATE_SHIELDED
    // bool isInvulnerable;    → stateFlags & ENEMY_STATE_INVULNERABLE
    // bool hasPulsed;         → stateFlags & ENEMY_STATE_PULSED
    // int phase;              → stateData.phase
    // int splitCount;         → stateData.splitCount
    // float shieldHealth;     → stateData.shieldHealth
    // float phaseTimer;       → stateData.phaseTimer
    // float transformTimer;   → stateData.transformTimer
    // float stormCycleTimer;  → stateData.stormCycleTimer

    // Special ability timers
    float teleportCooldown;
    float repulsionStrength;
    Vector2 orbitCenter;
    float orbitAngle;
    float orbitRadius;
} Enemy;
```

**Validation**:
```bash
# Check header compiles
gcc -c src/entities/enemy.h -I/opt/homebrew/Cellar/raylib/5.5/include -o /tmp/enemy.h.gch
# Expected: No errors (deprecated fields still present for compatibility)
```

---

### Phase 3: Initialization Code Update

**Estimated Time**: 10 minutes

#### Step 3.1: Update InitEnemyByType()

**File**: `src/entities/enemy.c`

Find `InitEnemyByType()` function and update each enemy type initialization:

**Before (example for BLACKHOLE)**:
```c
case ENEMY_TYPE_BLACKHOLE:
    enemy.isInvulnerable = true;
    enemy.hasPulsed = false;
    enemy.phase = 0;
    enemy.transformTimer = 0.0f;
    enemy.stormCycleTimer = 0.0f;
    break;
```

**After**:
```c
case ENEMY_TYPE_BLACKHOLE:
    enemy.stateFlags = ENEMY_STATE_INVULNERABLE;  // Start invulnerable
    enemy.stateData.phase = 0;
    enemy.stateData.transformTimer = 0.0f;
    enemy.stateData.stormCycleTimer = 0.0f;
    // hasPulsed = false is default (flag not set)
    break;
```

**Before (example for BOSS_1)**:
```c
case ENEMY_TYPE_BOSS_1:
    enemy.hasShield = true;
    enemy.shieldHealth = 200.0f;
    enemy.phase = 0;
    enemy.phaseTimer = 0.0f;
    break;
```

**After**:
```c
case ENEMY_TYPE_BOSS_1:
    enemy.stateFlags = ENEMY_STATE_SHIELDED;  // Start with shield
    enemy.stateData.shieldHealth = 200.0f;
    enemy.stateData.phase = 0;
    enemy.stateData.phaseTimer = 0.0f;
    break;
```

**Before (example for SPLITTER)**:
```c
case ENEMY_TYPE_SPLITTER:
    enemy.splitCount = 3;
    break;
```

**After**:
```c
case ENEMY_TYPE_SPLITTER:
    enemy.stateData.splitCount = 3;
    break;
```

**Validation**:
```bash
# Compile enemy.c
make src/entities/enemy.o
# Expected: No errors
```

---

### Phase 4: Usage Site Conversion (27 Locations)

**Estimated Time**: 20 minutes

#### Step 4.1: Convert enemy.c (24 locations)

**Pattern 1: Boolean Read**
```c
// Before
if (enemy->isInvulnerable)

// After
if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE))
```

**Pattern 2: Boolean Write (Set)**
```c
// Before
enemy->hasShield = true;

// After
SetState(&enemy->stateFlags, ENEMY_STATE_SHIELDED);
```

**Pattern 3: Boolean Write (Clear)**
```c
// Before
enemy->isInvulnerable = false;

// After
ClearState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);
```

**Pattern 4: Numeric Read**
```c
// Before
if (enemy->phase >= 2)

// After
if (enemy->stateData.phase >= 2)
```

**Pattern 5: Numeric Write**
```c
// Before
enemy->transformTimer += deltaTime;

// After
enemy->stateData.transformTimer += deltaTime;
```

**Specific Locations in enemy.c**:

1. **Line 332**: `enemy->isInvulnerable = true;`
   → `SetState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);`

2. **Line 341**: `if (enemy->isInvulnerable && enemy->phaseTimer > 1.5f)`
   → `if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE) && enemy->stateData.phaseTimer > 1.5f)`

3. **Line 342**: `enemy->isInvulnerable = false;`
   → `ClearState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);`

4. **Line 548**: `if (enemy->isInvulnerable)`
   → `if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE))`

5. **Line 768**: `if (enemy->isInvulnerable) return;`
   → `if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) return;`

6. **Line 771**: `if (enemy->hasShield && enemy->shieldHealth > 0)`
   → `if (HasState(enemy->stateFlags, ENEMY_STATE_SHIELDED) && enemy->stateData.shieldHealth > 0)`

7. **Line 772-773**: Shield damage logic
   ```c
   // Before
   enemy->shieldHealth -= damage;
   if (enemy->shieldHealth < 0) {
       enemy->health += enemy->shieldHealth;
       enemy->shieldHealth = 0;
       enemy->hasShield = false;
   }

   // After
   enemy->stateData.shieldHealth -= damage;
   if (enemy->stateData.shieldHealth < 0) {
       enemy->health += enemy->stateData.shieldHealth;
       enemy->stateData.shieldHealth = 0;
       ClearState(&enemy->stateFlags, ENEMY_STATE_SHIELDED);
   }
   ```

8. **Line 787**: `enemy->splitCount > 0`
   → `enemy->stateData.splitCount > 0`

**Continue pattern for all 24 locations**

**Validation (after each batch of 5-10 conversions)**:
```bash
make clean && make
# Expected: Compiles successfully
# Then test specific enemy types
./bin/game --test-mode
# Spawn and test converted enemy types
```

---

#### Step 4.2: Convert enemy_manager.c (3 locations)

**File**: `src/entities/managers/enemy_manager.c`

Search for state field accesses and convert using same patterns.

Example locations:
- Check for `enemy->phase` → `enemy->stateData.phase`
- Check for `enemy->isInvulnerable` → `HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)`

**Validation**:
```bash
make clean && make
# Expected: Compiles successfully
```

---

#### Step 4.3: Convert game.c BLACKHOLE Logic

**File**: `src/core/game.c` (around line 382)

**Before**:
```c
// Update blackhole state based on other enemies
if (otherEnemiesCount == 0 && game->enemies[i].isInvulnerable && !game->enemies[i].hasPulsed) {
    game->enemies[i].hasPulsed = true;
    game->enemies[i].isInvulnerable = false;
    game->enemies[i].movePattern = MOVE_PATTERN_TRACKING;
    game->enemies[i].color = (Color){150, 0, 50, 255};

    // Transformation logic...
}
```

**After**:
```c
// Update blackhole state based on other enemies
if (otherEnemiesCount == 0 &&
    HasState(game->enemies[i].stateFlags, ENEMY_STATE_INVULNERABLE) &&
    !HasState(game->enemies[i].stateFlags, ENEMY_STATE_PULSED)) {

    SetState(&game->enemies[i].stateFlags, ENEMY_STATE_PULSED);
    ClearState(&game->enemies[i].stateFlags, ENEMY_STATE_INVULNERABLE);
    game->enemies[i].movePattern = MOVE_PATTERN_TRACKING;
    game->enemies[i].color = (Color){150, 0, 50, 255};

    // Transformation logic...
}
```

**Validation**:
```bash
make clean && make
./bin/game --test-mode
# Spawn BLACKHOLE (press 0, click)
# Spawn another enemy, remove it
# BLACKHOLE should transform (color change to red, pulsed state)
```

---

### Phase 5: Test Mode Integration

**Estimated Time**: 15 minutes

#### Step 5.1: Add State Visualization UI

**File**: `src/core/dev_test_mode.c`

Add new function at end of file:

```c
/**
 * @brief Draw enemy state debug info
 * @param enemy Enemy to display state for
 * @param position Screen position for debug panel
 */
void DrawEnemyStateDebug(Enemy* enemy, Vector2 position) {
    int yOffset = 0;

    // Draw state flags as hex
    char flagsText[32];
    sprintf(flagsText, "Flags: 0x%02X", enemy->stateFlags);
    DrawText(flagsText, position.x, position.y + yOffset, 12, YELLOW);
    yOffset += 15;

    // Draw active boolean states
    if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) {
        DrawText("[INVULNERABLE]", position.x, position.y + yOffset, 10, ORANGE);
        yOffset += 12;
    }
    if (HasState(enemy->stateFlags, ENEMY_STATE_SHIELDED)) {
        DrawText("[SHIELDED]", position.x, position.y + yOffset, 10, SKYBLUE);
        yOffset += 12;
    }
    if (HasState(enemy->stateFlags, ENEMY_STATE_PULSED)) {
        DrawText("[PULSED]", position.x, position.y + yOffset, 10, GREEN);
        yOffset += 12;
    }
    if (HasState(enemy->stateFlags, ENEMY_STATE_STORM_ACTIVE)) {
        DrawText("[STORM]", position.x, position.y + yOffset, 10, RED);
        yOffset += 12;
    }

    // Draw numeric state data (only non-zero values)
    if (enemy->stateData.phase > 0) {
        char phaseText[32];
        sprintf(phaseText, "Phase: %d", enemy->stateData.phase);
        DrawText(phaseText, position.x, position.y + yOffset, 10, WHITE);
        yOffset += 12;
    }

    if (enemy->stateData.splitCount > 0) {
        char splitText[32];
        sprintf(splitText, "Splits: %d", enemy->stateData.splitCount);
        DrawText(splitText, position.x, position.y + yOffset, 10, WHITE);
        yOffset += 12;
    }

    if (enemy->stateData.shieldHealth > 0) {
        char shieldText[32];
        sprintf(shieldText, "Shield: %.0f", enemy->stateData.shieldHealth);
        DrawText(shieldText, position.x, position.y + yOffset, 10, SKYBLUE);
        yOffset += 12;
    }

    if (enemy->stateData.transformTimer > 0) {
        char transformText[32];
        sprintf(transformText, "Transform: %.1fs", enemy->stateData.transformTimer);
        DrawText(transformText, position.x, position.y + yOffset, 10, WHITE);
        yOffset += 12;
    }

    if (enemy->stateData.stormCycleTimer > 0) {
        char stormText[32];
        sprintf(stormText, "Storm: %.1fs", enemy->stateData.stormCycleTimer);
        DrawText(stormText, position.x, position.y + yOffset, 10, RED);
        yOffset += 12;
    }
}
```

Update `DrawTestModeUI()` to call state visualization:

```c
void DrawTestModeUI(TestModeState* state, int screenWidth, int screenHeight) {
    // ... existing enemy selector UI ...

    // NEW: Draw state debug for nearest enemy to mouse
    Vector2 mousePos = GetMousePosition();
    // Find nearest enemy (reuse RemoveNearestEnemy logic)
    // Then call DrawEnemyStateDebug() for that enemy

    // ... existing help overlay ...
}
```

**Validation**:
```bash
make clean && make
./bin/game --test-mode
# Spawn BLACKHOLE (press 0, click)
# Hover mouse over it - should see:
#   Flags: 0x01
#   [INVULNERABLE]
# Spawn another enemy, remove it
# BLACKHOLE should show:
#   Flags: 0x04
#   [PULSED]
```

---

#### Step 5.2: Add State Manipulation Shortcuts

**File**: `src/core/dev_test_mode.c`

Update `HandleTestModeKeyboard()` to add state toggle keys:

```c
void HandleTestModeKeyboard(TestModeState* state, void* gamePtr) {
    Game* game = (Game*)gamePtr;

    // ... existing keyboard handling ...

    // NEW: State manipulation shortcuts
    // Find enemy nearest to mouse
    Vector2 mousePos = GetMousePosition();
    int nearestEnemyIndex = -1;
    float nearestDistance = 999999.0f;

    for (int i = 0; i < game->enemyCount; i++) {
        float distance = Vector2Distance(game->enemies[i].position, mousePos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestEnemyIndex = i;
        }
    }

    // Toggle states for nearest enemy (if within 100px)
    if (nearestEnemyIndex >= 0 && nearestDistance < 100.0f) {
        Enemy* enemy = &game->enemies[nearestEnemyIndex];

        // I key: Toggle INVULNERABLE
        if (IsKeyPressed(KEY_I)) {
            ToggleState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);
        }

        // S key: Toggle SHIELDED
        if (IsKeyPressed(KEY_S)) {
            ToggleState(&enemy->stateFlags, ENEMY_STATE_SHIELDED);
            if (HasState(enemy->stateFlags, ENEMY_STATE_SHIELDED)) {
                enemy->stateData.shieldHealth = 200.0f;
            }
        }

        // P key: Increment phase (for bosses)
        if (IsKeyPressed(KEY_P)) {
            enemy->stateData.phase = (enemy->stateData.phase + 1) % 3;
        }
    }
}
```

Update help overlay to document new keys.

**Validation**:
```bash
make clean && make
./bin/game --test-mode
# Spawn BOSS_1 (press 6, click)
# Hover over boss, press I → should toggle invulnerable state
# Press S → should toggle shield
# Press P → should cycle phase 0 → 1 → 2 → 0
```

---

### Phase 6: Validation and Cleanup

**Estimated Time**: 15 minutes

#### Step 6.1: Build Validation

```bash
# Clean desktop build
make clean
make

# Expected output:
# - No compiler warnings
# - No errors
# - All .o files compile successfully
# - bin/game executable created

# Clean web build
make -f Makefile.web clean
make -f Makefile.web

# Expected output:
# - No emcc warnings
# - particle_storm.html, .js, .wasm files generated
# - File sizes reasonable (< 5MB for .wasm)
```

---

#### Step 6.2: Functional Testing

**Desktop Tests**:
```bash
./bin/game --test-mode

# Test 1: BLACKHOLE transformation
# 1. Press 0, click to spawn BLACKHOLE
# 2. Hover mouse over it → should show [INVULNERABLE]
# 3. Spawn another enemy (press 1, click)
# 4. Press R to remove the other enemy
# 5. BLACKHOLE should transform → [PULSED] state visible
# Result: PASS if transformation visible in UI

# Test 2: BOSS phase changes
# 1. Press 6, click to spawn BOSS_1
# 2. Damage boss until health < 70%
# 3. Should see Phase: 1 in state UI
# 4. Damage until health < 30%
# 5. Should see Phase: 2 in state UI
# Result: PASS if phases display correctly

# Test 3: SPLITTER split count
# 1. Press 4, click to spawn SPLITTER
# 2. Should see Splits: 3 in state UI
# 3. Damage until split
# 4. Child splitters should show Splits: 2
# Result: PASS if split count tracks correctly

# Test 4: Shield mechanics
# 1. Press 6, click to spawn BOSS_1
# 2. Should see [SHIELDED] and Shield: 200
# 3. Damage boss with particles
# 4. Shield value should decrease
# 5. When shield reaches 0, [SHIELDED] should disappear
# Result: PASS if shield state updates correctly
```

**Web Tests**:
```bash
python3 -m http.server 8000
# Open: http://localhost:8000/particle_storm.html?test-mode=1
# Repeat all desktop tests above
# Result: PASS if all tests work identically on web
```

---

#### Step 6.3: Memory Size Validation

```c
// Add temporary debug code to enemy.c
#include <stdio.h>

void CheckEnemyStructSize(void) {
    printf("Enemy struct size: %zu bytes\n", sizeof(Enemy));
    printf("  stateFlags: %zu bytes\n", sizeof(uint32_t));
    printf("  stateData: %zu bytes\n", sizeof(EnemyStateData));
}
```

```bash
# Run game and check output
./bin/game --test-mode

# Expected output:
# Enemy struct size: ~XXX bytes (original size + ~30-40 bytes)
# stateFlags: 4 bytes
# stateData: 28 bytes (int=4, int=4, float=4, float=4, float=4, float=4)

# Total increase: ~32 bytes (well under 50 byte limit)
```

---

#### Step 6.4: Remove Deprecated Fields

**File**: `src/entities/enemy.h`

After all conversions verified working, remove commented deprecated fields:

```c
typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    float health;
    EnemyType type;
    float speed;
    int scoreValue;
    MovePattern movePattern;
    Vector2 targetPosition;
    float stateChangeTimer;

    // Unified state system
    uint32_t stateFlags;      // Bitflags for boolean states
    EnemyStateData stateData; // Numeric state values

    // Special ability timers
    float teleportCooldown;
    float repulsionStrength;
    Vector2 orbitCenter;
    float orbitAngle;
    float orbitRadius;
} Enemy;
```

**Validation**:
```bash
# Rebuild everything
make clean && make

# If build succeeds, deprecated fields safely removed
```

---

#### Step 6.5: Update Documentation

**File**: `PLANNING.md`

Add new section under "Enemy Types Reference":

```markdown
## Enemy State System

### State Management Architecture

Enemies use a hybrid state system combining **bitflags** for boolean states and a **struct** for numeric values.

**State Flags** (`uint32_t stateFlags`):
- `ENEMY_STATE_INVULNERABLE` (0x01): Cannot take damage
- `ENEMY_STATE_SHIELDED` (0x02): Has active shield
- `ENEMY_STATE_PULSED` (0x04): BLACKHOLE has pulsed once
- `ENEMY_STATE_TELEPORTING` (0x08): Teleporter ability active
- `ENEMY_STATE_STORM_ACTIVE` (0x10): BLACKHOLE storm cycle active
- `ENEMY_STATE_SPLITTING` (0x20): SPLITTER actively splitting
- `ENEMY_STATE_PHASE_CHANGE` (0x40): BOSS phase transition

**State Data** (`EnemyStateData stateData`):
- `phase`: Boss phase (0, 1, 2)
- `splitCount`: Splitter splits remaining (0-3)
- `shieldHealth`: Shield HP (0.0-max)
- `phaseTimer`: Phase transition timer
- `transformTimer`: BLACKHOLE transformation timer
- `stormCycleTimer`: BLACKHOLE storm cycle timer (0-10s)

**Usage Examples**:
```c
// Check state
if (HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE)) { ... }

// Set state
SetState(&enemy->stateFlags, ENEMY_STATE_SHIELDED);

// Clear state
ClearState(&enemy->stateFlags, ENEMY_STATE_INVULNERABLE);

// Access numeric data
enemy->stateData.phase = 2;
if (enemy->stateData.shieldHealth > 0) { ... }
```

### Test Mode State Visualization

In developer test mode (`./bin/game --test-mode`), enemy states are displayed:
- Hover mouse over enemy to see state panel
- State flags shown as hex (e.g., `0x05` = INVULNERABLE + PULSED)
- Active boolean states shown as badges
- Numeric values shown when non-zero

**State Manipulation Keys** (in test mode):
- `I`: Toggle invulnerable (hover over enemy)
- `S`: Toggle shield
- `P`: Cycle phase (0 → 1 → 2)
```

---

### Phase 7: Unit Testing

**Estimated Time**: 10 minutes

#### Step 7.1: Create State System Tests

**File**: `tests/unit/test_enemy_state.c`

```c
#include "../../src/minunit/minunit.h"
#include "../../src/entities/enemy_state.h"

/**
 * Test: HasState() detects set flags
 */
MU_TEST(test_has_state) {
    uint32_t flags = ENEMY_STATE_INVULNERABLE | ENEMY_STATE_SHIELDED;

    mu_assert(HasState(flags, ENEMY_STATE_INVULNERABLE),
              "Should detect INVULNERABLE");
    mu_assert(HasState(flags, ENEMY_STATE_SHIELDED),
              "Should detect SHIELDED");
    mu_assert(!HasState(flags, ENEMY_STATE_PULSED),
              "Should not detect PULSED");
}

/**
 * Test: SetState() sets flags
 */
MU_TEST(test_set_state) {
    uint32_t flags = ENEMY_STATE_NONE;

    SetState(&flags, ENEMY_STATE_INVULNERABLE);
    mu_assert(HasState(flags, ENEMY_STATE_INVULNERABLE),
              "INVULNERABLE should be set");

    SetState(&flags, ENEMY_STATE_SHIELDED);
    mu_assert(HasState(flags, ENEMY_STATE_INVULNERABLE),
              "INVULNERABLE should remain set");
    mu_assert(HasState(flags, ENEMY_STATE_SHIELDED),
              "SHIELDED should be set");
}

/**
 * Test: ClearState() clears flags
 */
MU_TEST(test_clear_state) {
    uint32_t flags = ENEMY_STATE_INVULNERABLE | ENEMY_STATE_SHIELDED;

    ClearState(&flags, ENEMY_STATE_INVULNERABLE);
    mu_assert(!HasState(flags, ENEMY_STATE_INVULNERABLE),
              "INVULNERABLE should be cleared");
    mu_assert(HasState(flags, ENEMY_STATE_SHIELDED),
              "SHIELDED should remain set");
}

/**
 * Test: ToggleState() toggles flags
 */
MU_TEST(test_toggle_state) {
    uint32_t flags = ENEMY_STATE_NONE;

    ToggleState(&flags, ENEMY_STATE_PULSED);
    mu_assert(HasState(flags, ENEMY_STATE_PULSED),
              "PULSED should be set after toggle");

    ToggleState(&flags, ENEMY_STATE_PULSED);
    mu_assert(!HasState(flags, ENEMY_STATE_PULSED),
              "PULSED should be cleared after second toggle");
}

/**
 * Test: GetStateName() returns correct names
 */
MU_TEST(test_get_state_name) {
    mu_check(strcmp(GetStateName(ENEMY_STATE_INVULNERABLE), "INVULNERABLE") == 0);
    mu_check(strcmp(GetStateName(ENEMY_STATE_SHIELDED), "SHIELDED") == 0);
    mu_check(strcmp(GetStateName(ENEMY_STATE_PULSED), "PULSED") == 0);
}

/**
 * Test: Multiple state flags work together
 */
MU_TEST(test_multiple_states) {
    uint32_t flags = ENEMY_STATE_NONE;

    SetState(&flags, ENEMY_STATE_INVULNERABLE);
    SetState(&flags, ENEMY_STATE_SHIELDED);
    SetState(&flags, ENEMY_STATE_PULSED);

    mu_assert(HasState(flags, ENEMY_STATE_INVULNERABLE), "State 1");
    mu_assert(HasState(flags, ENEMY_STATE_SHIELDED), "State 2");
    mu_assert(HasState(flags, ENEMY_STATE_PULSED), "State 3");

    ClearState(&flags, ENEMY_STATE_SHIELDED);

    mu_assert(HasState(flags, ENEMY_STATE_INVULNERABLE), "State 1 remains");
    mu_assert(!HasState(flags, ENEMY_STATE_SHIELDED), "State 2 cleared");
    mu_assert(HasState(flags, ENEMY_STATE_PULSED), "State 3 remains");
}

/**
 * Test: EnemyStateData struct initialization
 */
MU_TEST(test_state_data_init) {
    EnemyStateData data = {0};

    mu_assert_int_eq(0, data.phase);
    mu_assert_int_eq(0, data.splitCount);
    mu_assert_double_eq(0.0, data.shieldHealth);
    mu_assert_double_eq(0.0, data.phaseTimer);
}

/**
 * Test suite
 */
MU_TEST_SUITE(enemy_state_suite) {
    MU_RUN_TEST(test_has_state);
    MU_RUN_TEST(test_set_state);
    MU_RUN_TEST(test_clear_state);
    MU_RUN_TEST(test_toggle_state);
    MU_RUN_TEST(test_get_state_name);
    MU_RUN_TEST(test_multiple_states);
    MU_RUN_TEST(test_state_data_init);
}

/**
 * Main test runner
 */
int main() {
    MU_RUN_SUITE(enemy_state_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

**Validation**:
```bash
# Compile and run tests
gcc -o tests/unit/test_enemy_state_compiled \
    tests/unit/test_enemy_state.c \
    src/entities/enemy_state.c \
    -I/opt/homebrew/Cellar/raylib/5.5/include

./tests/unit/test_enemy_state_compiled

# Expected output:
# "7 tests, N assertions, 0 failures"
```

---

## Anti-Patterns to Avoid

- ❌ Don't use `enemy->isInvulnerable` after refactoring - use `HasState()`
- ❌ Don't forget to use `&` when calling `SetState(&enemy->stateFlags, ...)`
- ❌ Don't add new boolean fields to Enemy struct - use stateFlags
- ❌ Don't use magic numbers for flags - use enum constants
- ❌ Don't skip inline keyword on helper functions - performance critical
- ❌ Don't batch all 27 conversions without testing - do 5-10 at a time
- ❌ Don't remove deprecated fields until all conversions verified working
- ❌ Don't forget to update both `Makefile` and `Makefile.web`

---

## Task Checklist (Implementation Order)

```markdown
Phase 1: New State System
- [ ] Create src/entities/enemy_state.h (~80 lines)
- [ ] Create src/entities/enemy_state.c (~50 lines)
- [ ] Validate header compiles standalone
- [ ] Validate implementation compiles

Phase 2: Enemy Struct
- [ ] Add #include "enemy_state.h" to enemy.h
- [ ] Add stateFlags and stateData fields
- [ ] Comment out deprecated fields (don't remove yet)
- [ ] Validate enemy.h compiles

Phase 3: Initialization
- [ ] Update InitEnemyByType() for BLACKHOLE
- [ ] Update InitEnemyByType() for BOSS_1
- [ ] Update InitEnemyByType() for BOSS_FINAL
- [ ] Update InitEnemyByType() for SPLITTER
- [ ] Update InitEnemyByType() for all other types
- [ ] Validate enemy.c compiles

Phase 4: Usage Conversion (27 locations)
- [ ] Convert enemy.c lines 332-342 (invulnerable logic)
- [ ] Convert enemy.c line 548 (invulnerable check)
- [ ] Convert enemy.c lines 768-776 (damage + shield)
- [ ] Convert enemy.c line 787 (split count)
- [ ] Convert remaining 20 locations in enemy.c
- [ ] Convert enemy_manager.c (3 locations)
- [ ] Convert game.c BLACKHOLE transformation (3 locations)
- [ ] Test after each 5-10 conversions

Phase 5: Test Mode Integration
- [ ] Add DrawEnemyStateDebug() to dev_test_mode.c
- [ ] Update DrawTestModeUI() to call state debug
- [ ] Add state manipulation keys (I, S, P)
- [ ] Update help overlay with new keys
- [ ] Test state visualization with all enemy types

Phase 6: Validation
- [ ] Desktop build: make clean && make (no warnings)
- [ ] Web build: make -f Makefile.web clean && make -f Makefile.web
- [ ] Test BLACKHOLE transformation in test mode
- [ ] Test BOSS phase changes
- [ ] Test SPLITTER split count
- [ ] Test shield mechanics
- [ ] Verify Enemy struct size increase < 50 bytes
- [ ] Remove deprecated fields from enemy.h
- [ ] Rebuild and verify

Phase 7: Documentation & Testing
- [ ] Create tests/unit/test_enemy_state.c
- [ ] Run unit tests (7 tests should pass)
- [ ] Update PLANNING.md with state system docs
- [ ] Update TASK.md with completion entry
- [ ] Add Makefile targets for enemy_state.c
- [ ] Add Makefile.web targets for enemy_state.c

Phase 8: Final Validation
- [ ] All unit tests pass
- [ ] Desktop build works (60 FPS maintained)
- [ ] Web build works
- [ ] No memory leaks (run with -fsanitize=address)
- [ ] All 11 enemy types work correctly
- [ ] Test mode state visualization works
- [ ] Git commit with descriptive message
```

---

## Validation Gates

### Gate 1: Compilation
```bash
# Desktop build
make clean && make
# Expected: 0 warnings, 0 errors

# Web build
make -f Makefile.web clean && make -f Makefile.web
# Expected: 0 warnings, 0 errors, files generated
```

### Gate 2: Unit Tests
```bash
gcc -o tests/unit/test_enemy_state_compiled \
    tests/unit/test_enemy_state.c \
    src/entities/enemy_state.c

./tests/unit/test_enemy_state_compiled
# Expected: "7 tests, N assertions, 0 failures"
```

### Gate 3: Functional - BLACKHOLE
```bash
./bin/game --test-mode
# 1. Press 0, click to spawn BLACKHOLE
# 2. Verify UI shows: Flags: 0x01, [INVULNERABLE]
# 3. Spawn another enemy (press 1, click)
# 4. Press R to remove other enemy
# 5. Verify BLACKHOLE transforms
# 6. Verify UI shows: Flags: 0x04, [PULSED]
# Expected: PASS - transformation visible
```

### Gate 4: Functional - BOSS Phases
```bash
./bin/game --test-mode
# 1. Press 6, click to spawn BOSS_1
# 2. Verify UI shows: [SHIELDED], Shield: 200
# 3. Damage until shield breaks
# 4. Verify [SHIELDED] disappears
# 5. Damage until health < 70%
# 6. Verify UI shows: Phase: 1
# 7. Damage until health < 30%
# 8. Verify UI shows: Phase: 2
# Expected: PASS - all phase transitions visible
```

### Gate 5: Performance
```bash
./bin/game --test-mode
# Spawn 20+ enemies (mix of types)
# Check FPS counter (top-left)
# Expected: 60 FPS maintained
```

### Gate 6: Memory Safety
```bash
make clean
make CFLAGS="-fsanitize=address -g"
./bin/game --test-mode
# Spawn 50+ enemies
# Remove all with C key
# Spawn 50 more
# Exit with ESC
# Expected: No memory leaks reported
```

---

## Confidence Score: 9/10

### Strengths

✅ **Complete Context**: All 27 usage locations identified via grep
✅ **Proven Patterns**: Bitflags are standard C technique, well-understood
✅ **Incremental Approach**: 6 phases with validation after each
✅ **Backward Compatible**: Deprecated fields remain until conversion complete
✅ **Performance Conscious**: Inline functions eliminate overhead
✅ **Comprehensive Testing**: Unit tests + functional tests + memory tests
✅ **Clear Validation Gates**: 6 executable validation checkpoints
✅ **Documentation**: Detailed conversion patterns for all cases
✅ **Existing Codebase Analysis**: Built on actual grep results, not assumptions

### Risks & Mitigation

⚠️ **Risk**: Missing some state field usages in grep search
- **Mitigation**: Grep pattern covers all 9 fields, verified 27 total matches
- **Additional check**: Compiler will fail if old fields used after removal

⚠️ **Risk**: Performance regression from helper function calls
- **Mitigation**: All helpers are `static inline`, compiler optimizes to direct bitwise ops
- **Validation**: Performance gate requires 60 FPS maintained

⚠️ **Risk**: Breaking existing enemy behaviors during conversion
- **Mitigation**: Test after every 5-10 conversions, keep deprecated fields until end
- **Rollback**: Git commit after each phase for easy rollback

### Why 9/10 (not 10/10)

- **-1 point**: Haven't seen actual inline function optimization in this codebase yet (unproven pattern)
- However, inline functions are standard C feature, widely used, low risk

---

## Summary

This PRP provides a complete refactoring path from scattered enemy state fields to a unified hybrid system (bitflags + struct). The implementation is broken into 6 manageable phases with clear validation gates after each. Expected implementation time is **60 minutes** as estimated in TASK.md.

**Key Benefits**:
1. Fixes BLACKHOLE test mode bug (invulnerability state now visible)
2. Enables comprehensive enemy state testing in test mode
3. Reduces coupling (9 fields → 2 fields)
4. Improves debugging (single hex value shows all boolean states)
5. Scales to future enemy types (just add enum values)

**Implementation Strategy**:
- Phase 1-2: Create foundation (new files, struct changes) - **15 min**
- Phase 3-4: Convert all usages incrementally - **30 min**
- Phase 5: Add test mode UI - **15 min**
- Phase 6-7: Validate and document - **10 min**

All validation gates are executable with clear pass/fail criteria. The refactoring is backward compatible (deprecated fields kept until end) and safe to implement incrementally with frequent testing.
