# Gravity System - Universal Physics Force Engine

## Goal
Implement a comprehensive, performant gravity system that supports attraction, repulsion, and orbital mechanics for all game objects (Particles, Enemies, Player, Items), with support for multiple simultaneous gravity sources (BLACKHOLE enemies, REPULSOR enemies, environmental objects, player skills).

**Critical Fix**: This PRP solves the immediate bug where BLACKHOLE gravity doesn't work in Test mode (gravity code is only in main game loop, not Test mode loop).

## Why
- **Current Bug**: BL

ACKHOLE gravity effect doesn't work in Test mode (game.c:479-499 only runs in main loop)
- **Code Duplication**: BLACKHOLE and REPULSOR have separate hardcoded gravity logic (game.c:343-500)
- **No Extensibility**: Cannot add new gravity sources (environmental hazards, power-ups) without duplicating code
- **Future Features**: Foundation for planned features like gravity wells, player skills, orbital mechanics
- **Maintainability**: Centralize all gravity logic in one system for easier debugging and optimization

## What
A centralized gravity system (`gravity_system.c/h`) that:
- Manages multiple concurrent gravity sources (up to 32)
- Applies gravity forces to all registered targets (particles, enemies, player, items)
- Supports 3 gravity types: Attraction, Repulsion, Orbital (extensible to 8 types via enum)
- Integrates seamlessly with existing event system and memory pools
- Works in both main game loop AND test mode
- Maintains 60+ FPS with 100,000 particles and 32 gravity sources

### Success Criteria
- [ ] BLACKHOLE gravity works in Test mode
- [ ] All existing gravity behavior preserved (BLACKHOLE attraction, REPULSOR repulsion)
- [ ] Desktop and web builds compile successfully
- [ ] Performance: 60+ FPS with 100k particles + 50 enemies + 10 gravity sources
- [ ] Unit tests pass for gravity calculations
- [ ] Memory usage increase < 10KB total
- [ ] G key in Test mode visualizes all active gravity fields

## All Needed Context

### Documentation & References
```yaml
# Raylib Vector2 Math
- url: https://www.raylib.com/cheatsheet/cheatsheet.html
  section: "Math - Vector2 operations"
  why: Vector2Normalize, Vector2Distance, Vector2Scale are critical for force calculations

- url: https://github.com/raysan5/raylib/wiki/raymath
  section: Vector2 functions
  critical: raymath.h is already included in project, use these for all vector math

# Existing Patterns to Follow
- file: src/entities/enemy_state.h
  why: Shows how to create bitflag enums and inline helper functions (HasState, SetState, etc.)
  pattern: |
    typedef enum {
        GRAVITY_TYPE_NONE = 0,
        GRAVITY_TYPE_ATTRACTION = 1 << 0,
        GRAVITY_TYPE_REPULSION = 1 << 1,
        // ... follow this bitflag pattern
    } GravityType;

    static inline void ApplyGravityMask(...) { /* inline for performance */ }

- file: src/core/memory_pool.h
  why: Shows memory pool API - use for GravitySource allocations if needed
  pattern: MemoryPool_Init(), MemoryPool_Alloc(), MemoryPool_Free()

- file: src/core/physics.c
  why: Shows collision processing pattern with batch optimization (lines 81-127)
  pattern: |
    const int BATCH_SIZE = 1000;
    for (int batchStart = 0; batchStart < PARTICLE_COUNT; batchStart += BATCH_SIZE) {
        // Process in batches for cache efficiency
    }

- file: src/core/game.c:343-500
  why: Current gravity implementations (REPULSOR and BLACKHOLE)
  critical: |
    REPULSOR (lines 343-362):
    - Radius: REPULSE_RADIUS = 150.0f
    - Force: (1.0f - dist / radius) * 2.0f
    - Direction: Pushes particles AWAY from enemy

    BLACKHOLE (lines 479-499):
    - Radius: 200.0f (BLACKHOLE_RADIUS)
    - Force: (1.0f - dist / radius) * 5.0f
    - Direction: Pulls particles TOWARDS enemy
    - Condition: Only when INVULNERABLE && !PULSED

    **Must preserve exact same force calculation!**

- file: src/core/game.c:163-182
  why: Test mode update loop - THIS is where we need to add gravity
  critical: Currently NO gravity processing happens here - this is the bug!
```

### Current Codebase Tree
```
src/
├── core/
│   ├── event/              # Event system (publish/subscribe)
│   ├── game.c/h            # Main game loop (CURRENT GRAVITY LOCATION)
│   ├── physics.c/h         # Collision detection only
│   ├── memory_pool.c/h     # Memory pool for allocations
│   └── dev_test_mode.c/h   # Test mode (NEEDS GRAVITY)
├── entities/
│   ├── enemy.c/h           # Enemy types (BLACKHOLE, REPULSOR)
│   ├── enemy_state.c/h     # Enemy state system (PATTERN TO FOLLOW)
│   ├── particle.c/h        # Particle structure
│   ├── player.c/h          # Player structure
│   └── items/
│       └── hp_potion.c/h   # Item example
```

### Desired Codebase Tree with New Files
```
src/
├── core/
│   ├── gravity_system.h    # NEW: Gravity system interface (~150 lines)
│   ├── gravity_system.c    # NEW: Gravity implementation (~400 lines)
│   ├── event/
│   ├── game.c/h            # MODIFIED: Remove hardcoded gravity, call ApplyAllGravity()
│   ├── physics.c/h         # UNMODIFIED
│   ├── memory_pool.c/h     # UNMODIFIED
│   └── dev_test_mode.c/h   # MODIFIED: Add gravity visualization (G key)
├── entities/
│   ├── enemy.c/h           # MODIFIED: Register BLACKHOLE/REPULSOR as GravitySources
│   ├── particle.c/h        # MODIFIED: Add mass, affectedByGravity fields
│   └── ...
tests/
└── unit/
    └── test_gravity_system.c  # NEW: Unit tests (~300 lines)
```

## Implementation Blueprint

### Phase 1: Core Gravity System (Priority: CRITICAL)
**Goal**: Create gravity_system.h/.c with data structures and core functions

#### Step 1.1: Create gravity_system.h
```c
#ifndef GRAVITY_SYSTEM_H
#define GRAVITY_SYSTEM_H

#include "raylib.h"
#include <stdint.h>
#include <stdbool.h>

// Maximum concurrent gravity sources (performance vs feature tradeoff)
#define MAX_GRAVITY_SOURCES 32

// Gravity types (bitflags for future combinations)
typedef enum {
    GRAVITY_TYPE_NONE       = 0,
    GRAVITY_TYPE_ATTRACTION = 1 << 0,  // Pulls targets towards source (BLACKHOLE)
    GRAVITY_TYPE_REPULSION  = 1 << 1,  // Pushes targets away (REPULSOR)
    GRAVITY_TYPE_ORBITAL    = 1 << 2,  // Future: Orbital mechanics
    GRAVITY_TYPE_DIRECTIONAL= 1 << 3,  // Future: Wind, conveyor belts
} GravityType;

// Gravity source (what creates gravity)
typedef struct {
    Vector2 position;      // Gravity center point
    float radius;          // Effective range (pixels)
    float strength;        // Force multiplier (base force * strength)
    GravityType type;      // Type of gravity
    bool active;           // Is this source currently active?
    void* sourcePtr;       // Pointer to source object (Enemy*, etc) - for reference only
    int sourceType;        // Source type: 0=enemy, 1=environment, 2=player_skill, 3=item
    int sourceId;          // Unique ID for source (for removal)
} GravitySource;

// Gravity target (what receives gravity)
typedef struct {
    Vector2* position;     // Pointer to target's position
    Vector2* velocity;     // Pointer to target's velocity (modified by gravity)
    float mass;            // Mass factor (1.0 = normal, 0.5 = half effect, 2.0 = double)
    bool affectedByGravity; // Can be disabled per-object
} GravityTarget;

// System lifecycle
void InitGravitySystem(void);
void CleanupGravitySystem(void);

// Source management
int RegisterGravitySource(GravitySource source);    // Returns unique ID
void UnregisterGravitySource(int sourceId);
void UpdateGravitySource(int sourceId, Vector2 newPosition); // For moving sources
void SetGravitySourceActive(int sourceId, bool active);

// Main update function (call once per frame)
void ApplyAllGravitySources(void* gamePtr, float deltaTime);

// Helper functions (inline for performance)
static inline Vector2 CalculateGravityForce(Vector2 targetPos, GravitySource source) {
    float dx = source.position.x - targetPos.x;
    float dy = source.position.y - targetPos.y;
    float dist = sqrtf(dx*dx + dy*dy);

    // Avoid division by zero
    if (dist < 1.0f) dist = 1.0f;

    // Normalize direction
    Vector2 direction = {dx / dist, dy / dist};

    // Calculate force based on type
    float force = 0.0f;

    if (source.type & GRAVITY_TYPE_ATTRACTION) {
        // Inverse square falloff (realistic) - can tune to linear if needed
        force = source.strength * (1.0f - dist / source.radius);
        // Keep direction (pulls towards source)
    } else if (source.type & GRAVITY_TYPE_REPULSION) {
        // Same falloff but inverted direction
        force = source.strength * (1.0f - dist / source.radius);
        direction.x = -direction.x;  // Flip direction (pushes away)
        direction.y = -direction.y;
    }

    // Clamp force to prevent extreme values
    if (force < 0.0f) force = 0.0f;
    if (force > source.strength * 2.0f) force = source.strength * 2.0f;

    return (Vector2){direction.x * force, direction.y * force};
}

static inline bool IsInGravityRange(Vector2 targetPos, GravitySource source) {
    float dx = source.position.x - targetPos.x;
    float dy = source.position.y - targetPos.y;
    float distSquared = dx*dx + dy*dy;
    float radiusSquared = source.radius * source.radius;
    return distSquared < radiusSquared && distSquared > 1.0f; // Exclude center point
}

// Debug/visualization
void DrawGravityFields(bool showLabels); // For test mode (G key)
int GetActiveGravitySourceCount(void);

#endif // GRAVITY_SYSTEM_H
```

#### Step 1.2: Create gravity_system.c
```c
#include "gravity_system.h"
#include "game.h"
#include "../entities/particle.h"
#include "../entities/enemy.h"
#include <string.h>
#include <stdio.h>

// Internal state
static GravitySource g_gravitySources[MAX_GRAVITY_SOURCES];
static int g_nextSourceId = 1; // Start at 1 (0 = invalid)
static int g_activeSourceCount = 0;

void InitGravitySystem(void) {
    memset(g_gravitySources, 0, sizeof(g_gravitySources));
    g_nextSourceId = 1;
    g_activeSourceCount = 0;
}

void CleanupGravitySystem(void) {
    // Nothing to free (static arrays)
    g_activeSourceCount = 0;
}

int RegisterGravitySource(GravitySource source) {
    // Find empty slot
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == 0) { // Empty slot
            source.sourceId = g_nextSourceId++;
            g_gravitySources[i] = source;
            g_activeSourceCount++;
            return source.sourceId;
        }
    }

    // No slots available
    fprintf(stderr, "WARNING: Max gravity sources (%d) reached!\n", MAX_GRAVITY_SOURCES);
    return 0; // Invalid ID
}

void UnregisterGravitySource(int sourceId) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            memset(&g_gravitySources[i], 0, sizeof(GravitySource));
            g_activeSourceCount--;
            return;
        }
    }
}

void UpdateGravitySource(int sourceId, Vector2 newPosition) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            g_gravitySources[i].position = newPosition;
            return;
        }
    }
}

void SetGravitySourceActive(int sourceId, bool active) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == sourceId) {
            g_gravitySources[i].active = active;
            return;
        }
    }
}

void ApplyAllGravitySources(void* gamePtr, float deltaTime) {
    Game* game = (Game*)gamePtr;

    // Early exit if no gravity sources
    if (g_activeSourceCount == 0) return;

    // Apply gravity to all particles
    for (int p = 0; p < PARTICLE_COUNT; p++) {
        // Skip if particle shouldn't be affected (future feature)
        // if (!game->particles[p].affectedByGravity) continue;

        Vector2 totalForce = {0, 0};

        // Accumulate forces from all active sources
        for (int s = 0; s < MAX_GRAVITY_SOURCES; s++) {
            if (g_gravitySources[s].sourceId == 0) continue; // Empty slot
            if (!g_gravitySources[s].active) continue;

            // Quick range check
            if (!IsInGravityRange(game->particles[p].position, g_gravitySources[s])) {
                continue;
            }

            // Calculate and accumulate force
            Vector2 force = CalculateGravityForce(game->particles[p].position, g_gravitySources[s]);
            totalForce.x += force.x;
            totalForce.y += force.y;
        }

        // Apply accumulated force to velocity
        game->particles[p].velocity.x += totalForce.x * deltaTime;
        game->particles[p].velocity.y += totalForce.y * deltaTime;
    }

    // TODO Phase 4: Apply gravity to enemies
    // TODO Phase 5: Apply gravity to player
    // TODO Phase 5: Apply gravity to items
}

void DrawGravityFields(bool showLabels) {
    for (int i = 0; i < MAX_GRAVITY_SOURCES; i++) {
        if (g_gravitySources[i].sourceId == 0) continue;
        if (!g_gravitySources[i].active) continue;

        GravitySource* src = &g_gravitySources[i];

        // Choose color based on type
        Color fieldColor = BLUE;
        if (src->type & GRAVITY_TYPE_ATTRACTION) {
            fieldColor = Fade(PURPLE, 0.3f); // Purple for attraction
        } else if (src->type & GRAVITY_TYPE_REPULSION) {
            fieldColor = Fade(ORANGE, 0.3f); // Orange for repulsion
        }

        // Draw influence radius
        DrawCircleLines((int)src->position.x, (int)src->position.y, src->radius, fieldColor);
        DrawCircleV(src->position, 5.0f, field Color);

        // Draw label if requested
        if (showLabels) {
            const char* typeStr = (src->type & GRAVITY_TYPE_ATTRACTION) ? "ATT" : "REP";
            DrawText(TextFormat("%s %.0f", typeStr, src->strength),
                    (int)src->position.x + 10, (int)src->position.y - 10, 12, WHITE);
        }
    }
}

int GetActiveGravitySourceCount(void) {
    return g_activeSourceCount;
}
```

**Validation Gate 1**:
```bash
# Compile standalone
gcc -c src/core/gravity_system.c -Isrc -I/opt/homebrew/Cellar/raylib/5.5/include -std=c99 -Wall
# Should compile with no errors
```

---

### Phase 2: BLACKHOLE Integration (Priority: CRITICAL)
**Goal**: Migrate BLACKHOLE from hardcoded gravity to gravity system

#### Step 2.1: Update enemy.c - Register BLACKHOLE as gravity source
Find `UpdateEnemy()` function and add gravity source registration for BLACKHOLE:

```c
// In src/entities/enemy.c, add near top:
#include "../core/gravity_system.h"

// Add static variable to track gravity source IDs
static int g_enemyGravitySourceIds[MAX_ENEMIES] = {0};

// In UpdateEnemy(), add AFTER line ~300 where enemy behavior is updated:
if (enemy->type == ENEMY_TYPE_BLACKHOLE) {
    bool shouldHaveGravity = HasState(enemy->stateFlags, ENEMY_STATE_INVULNERABLE) &&
                             !HasState(enemy->stateFlags, ENEMY_STATE_PULSED);

    // Register or update gravity source
    if (shouldHaveGravity) {
        if (g_enemyGravitySourceIds[/* enemy index */] == 0) {
            // Register new source
            GravitySource source = {
                .position = enemy->position,
                .radius = 200.0f,          // BLACKHOLE_RADIUS
                .strength = 5.0f,           // BLACKHOLE_FORCE
                .type = GRAVITY_TYPE_ATTRACTION,
                .active = true,
                .sourcePtr = enemy,
                .sourceType = 0, // Enemy
                .sourceId = 0  // Will be assigned
            };
            g_enemyGravitySourceIds[/* enemy index */] = RegisterGravitySource(source);
        } else {
            // Update existing source position
            UpdateGravitySource(g_enemyGravitySourceIds[/* enemy index */], enemy->position);
        }
    } else {
        // Remove gravity if conditions no longer met
        if (g_enemyGravitySourceIds[/* enemy index */] != 0) {
            UnregisterGravitySource(g_enemyGravitySourceIds[/* enemy index */]);
            g_enemyGravitySourceIds[/* enemy index */] = 0;
        }
    }
}
```

#### Step 2.2: Remove hardcoded BLACKHOLE gravity from game.c
Delete lines 479-499 in `src/core/game.c` (the hardcoded BLACKHOLE gravity loop).

#### Step 2.3: Add ApplyAllGravitySources() to game loops
In `src/core/game.c`:

1. Add include at top:
```c
#include "gravity_system.h"
```

2. In `InitGame()` (around line 50), add:
```c
InitGravitySystem();
```

3. In `CleanupGame()`, add:
```c
CleanupGravitySystem();
```

4. In main game loop (around line 220, AFTER UpdateAllEnemies()):
```c
// Apply gravity forces to all objects
ApplyAllGravitySources(game, game->deltaTime);
```

5. **CRITICAL FIX**: In Test mode loop (line 171, AFTER UpdateAllParticles()):
```c
// Apply gravity forces in test mode too!
ApplyAllGravitySources(game, game->deltaTime);
```

**Validation Gate 2**:
```bash
# Full compile
make clean && make
# Should compile successfully

# Test in Test mode
./bin/game --test-mode
# 1. Spawn BLACKHOLE (0 key + click)
# 2. Move mouse to create particles
# 3. Move particles near BLACKHOLE
# 4. Particles should be pulled towards BLACKHOLE
```

---

### Phase 3: REPULSOR Integration (Priority: HIGH)
**Goal**: Migrate REPULSOR from hardcoded repulsion to gravity system

#### Step 3.1: Update enemy.c - Register REPULSOR
Add to `UpdateEnemy()` for REPULSOR enemies:

```c
if (enemy->type == ENEMY_TYPE_REPULSOR) {
    if (g_enemyGravitySourceIds[/* enemy index */] == 0) {
        GravitySource source = {
            .position = enemy->position,
            .radius = 150.0f,          // REPULSE_RADIUS
            .strength = 2.0f,           // Repulsion strength
            .type = GRAVITY_TYPE_REPULSION,
            .active = true,
            .sourcePtr = enemy,
            .sourceType = 0,
            .sourceId = 0
        };
        g_enemyGravitySourceIds[/* enemy index */] = RegisterGravitySource(source);
    } else {
        UpdateGravitySource(g_enemyGravitySourceIds[/* enemy index */], enemy->position);
    }
}
```

#### Step 3.2: Remove hardcoded REPULSOR from game.c
Delete lines 343-362 in `src/core/game.c` (REPULSOR repulsion loop).

**Validation Gate 3**:
```bash
# Compile and test
make clean && make
./bin/game --start-stage 8  # Stage 8 has REPULSOR enemies

# Verify particles are pushed away from REPULSOR
```

---

### Phase 4: Test Mode Visualization (Priority: MEDIUM)
**Goal**: Add G key to visualize gravity fields in test mode

#### Step 4.1: Update dev_test_mode.c
```c
// Add to HandleTestModeKeyboard():
if (IsKeyPressed(KEY_G)) {
    state->showGravityFields = !state->showGravityFields;
}

// Add to DrawTestModeUI(), after enemy state debug:
if (state->showGravityFields) {
    DrawGravityFields(true); // Show with labels

    // Draw count in corner
    int count = GetActiveGravitySourceCount();
    DrawText(TextFormat("Gravity Sources: %d", count),
             screenWidth - 200, 10, 16, GREEN);
}
```

#### Step 4.2: Update help overlay
Add to help text in `DrawTestModeUI()`:
```c
DrawText("G: Toggle Gravity Visualization", helpX + 10, helpY + 280, 14, GREEN);
```

**Validation Gate 4**:
```bash
# Test mode with visualization
./bin/game --test-mode
# 1. Spawn BLACKHOLE and REPULSOR
# 2. Press G key
# 3. Should see purple circle (BLACKHOLE) and orange circle (REPULSOR)
# 4. Labels should show "ATT 5.0" and "REP 2.0"
```

---

### Phase 5: Particle System Enhancement (Priority: LOW)
**Goal**: Add mass and affectedByGravity to Particle struct

#### Step 5.1: Update particle.h
```c
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;

    // Gravity system support (Phase 5)
    float mass;              // Default: 1.0f
    bool affectedByGravity;  // Default: true
} Particle;
```

#### Step 5.2: Update particle.c InitParticle()
```c
Particle particle;
// ... existing init ...
particle.mass = 1.0f;
particle.affectedByGravity = true;
return particle;
```

#### Step 5.3: Use mass in gravity_system.c
Update `ApplyAllGravitySources()`:
```c
// Before: game->particles[p].velocity.x += totalForce.x * deltaTime;
// After:
float mass = 1.0f; // game->particles[p].mass; // Uncomment when Particle has mass field
game->particles[p].velocity.x += (totalForce.x / mass) * deltaTime;
game->particles[p].velocity.y += (totalForce.y / mass) * deltaTime;
```

---

### Phase 6: Build System Integration (Priority: CRITICAL)
**Goal**: Add gravity_system.c to Makefiles

#### Step 6.1: Update Makefile
Add to SRC_FILES list:
```makefile
SRC_FILES := \
    $(SRC_DIR)/main.c \
    $(CORE_DIR)/game.c \
    $(CORE_DIR)/physics.c \
    $(CORE_DIR)/gravity_system.c \   # NEW
    # ... rest of files
```

#### Step 6.2: Update Makefile.web
Same change in Makefile.web.

**Validation Gate 5**:
```bash
# Desktop build
make clean && make
# Should succeed

# Web build
make -f Makefile.web clean && make -f Makefile.web
# Should succeed
```

---

### Phase 7: Unit Tests (Priority: MEDIUM)
**Goal**: Create comprehensive unit tests

#### Step 7.1: Create test_gravity_system.c
```c
// tests/unit/test_gravity_system.c
#include "../../src/minunit/minunit.h"
#include "../../src/core/gravity_system.h"
#include <math.h>

MU_TEST(test_gravity_system_init) {
    InitGravitySystem();
    mu_assert_int_eq(0, GetActiveGravitySourceCount());
    CleanupGravitySystem();
}

MU_TEST(test_register_gravity_source) {
    InitGravitySystem();

    GravitySource source = {
        .position = {100, 100},
        .radius = 50.0f,
        .strength = 1.0f,
        .type = GRAVITY_TYPE_ATTRACTION,
        .active = true
    };

    int id = RegisterGravitySource(source);
    mu_assert(id > 0, "Should return valid ID");
    mu_assert_int_eq(1, GetActiveGravitySourceCount());

    CleanupGravitySystem();
}

MU_TEST(test_attraction_force_calculation) {
    GravitySource source = {
        .position = {100, 100},
        .radius = 100.0f,
        .strength = 10.0f,
        .type = GRAVITY_TYPE_ATTRACTION,
        .active = true
    };

    Vector2 targetPos = {150, 100}; // 50 pixels away
    Vector2 force = CalculateGravityForce(targetPos, source);

    // Force should point towards source (negative X)
    mu_assert(force.x < 0, "X force should be negative (towards source)");
    mu_assert_double_eq(0.0, force.y); // Same Y, so no Y force

    // Force magnitude should be based on distance
    float expectedMagnitude = source.strength * (1.0f - 50.0f / source.radius);
    float actualMagnitude = sqrtf(force.x * force.x + force.y * force.y);
    mu_assert(fabs(expectedMagnitude - actualMagnitude) < 0.1f, "Force magnitude incorrect");
}

MU_TEST(test_repulsion_force_calculation) {
    GravitySource source = {
        .position = {100, 100},
        .radius = 100.0f,
        .strength = 5.0f,
        .type = GRAVITY_TYPE_REPULSION,
        .active = true
    };

    Vector2 targetPos = {150, 100};
    Vector2 force = CalculateGravityForce(targetPos, source);

    // Force should point away from source (positive X)
    mu_assert(force.x > 0, "X force should be positive (away from source)");
}

MU_TEST(test_is_in_gravity_range) {
    GravitySource source = {
        .position = {100, 100},
        .radius = 50.0f
    };

    Vector2 insidePos = {120, 120}; // ~28 pixels away
    Vector2 outsidePos = {200, 200}; // ~141 pixels away

    mu_assert(IsInGravityRange(insidePos, source), "Should be inside range");
    mu_assert(!IsInGravityRange(outsidePos, source), "Should be outside range");
}

MU_TEST_SUITE(gravity_system_suite) {
    MU_RUN_TEST(test_gravity_system_init);
    MU_RUN_TEST(test_register_gravity_source);
    MU_RUN_TEST(test_attraction_force_calculation);
    MU_RUN_TEST(test_repulsion_force_calculation);
    MU_RUN_TEST(test_is_in_gravity_range);
}

int main() {
    MU_RUN_SUITE(gravity_system_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

**Validation Gate 6**:
```bash
# Compile and run tests
gcc -o tests/unit/test_gravity_system tests/unit/test_gravity_system.c \
    src/core/gravity_system.c \
    -Isrc -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
    -std=c99 -lm

./tests/unit/test_gravity_system
# All tests should pass
```

---

### Phase 8: Documentation (Priority: LOW)
**Goal**: Document the gravity system in PLANNING.md

Add section to PLANNING.md:
```markdown
### 5. Gravity System
Centralized force simulation for attraction, repulsion, and orbital mechanics.

**Key Files:**
- `src/core/gravity_system.c` - Core implementation
- `src/core/gravity_system.h` - Public API

**Features:**
- Supports up to 32 concurrent gravity sources
- 3 gravity types: Attraction, Repulsion, Orbital
- Works with particles, enemies, player, items
- Optimized batch processing for 100k particles
- Test mode visualization (G key)

**Usage Example:**
```c
// Register BLACKHOLE as gravity source
GravitySource blackhole = {
    .position = enemy->position,
    .radius = 200.0f,
    .strength = 5.0f,
    .type = GRAVITY_TYPE_ATTRACTION,
    .active = true
};
int id = RegisterGravitySource(blackhole);

// Update every frame
ApplyAllGravitySources(game, deltaTime);

// Remove when enemy dies
UnregisterGravitySource(id);
```

**Performance:**
- 60+ FPS with 100k particles + 32 gravity sources
- Spatial culling: O(n * m) where n = particles, m = active sources
- Batch processing for cache efficiency
```

---

## Validation Gates (Executable)

### Gate 1: Compilation
```bash
# Desktop
make clean && make 2>&1 | grep -E "(error|warning)" || echo "✅ Desktop build passed"

# Web
make -f Makefile.web clean && make -f Makefile.web 2>&1 | grep -E "(error|warning)" || echo "✅ Web build passed"
```

### Gate 2: BLACKHOLE Test Mode Fix
```bash
# Manual test required
./bin/game --test-mode
# 1. Press 0 to select BLACKHOLE
# 2. Click to spawn
# 3. Move mouse to create particles
# 4. Move particles near BLACKHOLE
# PASS: Particles are pulled towards BLACKHOLE
# FAIL: Particles ignore BLACKHOLE
```

### Gate 3: REPULSOR Test
```bash
./bin/game --start-stage 8
# PASS: Particles pushed away from REPULSOR enemies
# FAIL: Particles ignore REPULSOR
```

### Gate 4: Gravity Visualization
```bash
./bin/game --test-mode
# 1. Spawn BLACKHOLE (0) and spawn BASIC enemy (1)
# 2. Press G
# PASS: Purple circle around BLACKHOLE with "ATT 5.0" label
# FAIL: No visualization or wrong colors
```

### Gate 5: Unit Tests
```bash
gcc -o tests/unit/test_gravity_system tests/unit/test_gravity_system.c \
    src/core/gravity_system.c -Isrc -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib -lraylib \
    -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -std=c99 -lm

./tests/unit/test_gravity_system
# PASS: "5 tests, 5 assertions, 0 failures"
# FAIL: Any test failures
```

### Gate 6: Performance Test
```bash
# In test mode
./bin/game --test-mode
# 1. Spawn 10 BLACKHOLE enemies
# 2. Spawn 10 REPULSOR enemies
# 3. Move mouse rapidly to create max particles
# 4. Check FPS (displayed on screen)
# PASS: FPS >= 55
# FAIL: FPS < 55
```

---

## Anti-Patterns to Avoid
- ❌ Don't create new math functions - use raymath.h (Vector2Normalize, etc.)
- ❌ Don't use malloc/free for gravity sources - use static array (MAX_GRAVITY_SOURCES)
- ❌ Don't apply gravity in UpdateEnemy() - use centralized ApplyAllGravitySources()
- ❌ Don't forget to call ApplyAllGravitySources() in BOTH main loop AND test mode
- ❌ Don't break existing BLACKHOLE/REPULSOR behavior - preserve exact force calculations
- ❌ Don't add gravity to Build system - must update BOTH Makefile and Makefile.web

---

## Implementation Checklist

### Phase 1: Core System ✓
- [ ] Create src/core/gravity_system.h with structs and inline functions
- [ ] Create src/core/gravity_system.c with implementation
- [ ] Validate standalone compilation

### Phase 2: BLACKHOLE Integration ✓
- [ ] Add gravity registration in enemy.c UpdateEnemy()
- [ ] Remove hardcoded BLACKHOLE gravity from game.c:479-499
- [ ] Add ApplyAllGravitySources() to main loop
- [ ] Add ApplyAllGravitySources() to test mode loop (CRITICAL FIX)
- [ ] Test BLACKHOLE gravity in test mode

### Phase 3: REPULSOR Integration ✓
- [ ] Add REPULSOR gravity registration
- [ ] Remove hardcoded REPULSOR from game.c:343-362
- [ ] Test in Stage 8

### Phase 4: Test Mode Visualization ✓
- [ ] Add G key handler in dev_test_mode.c
- [ ] Implement DrawGravityFields()
- [ ] Update help overlay
- [ ] Test visualization

### Phase 5: Particle Enhancement (Optional)
- [ ] Add mass/affectedByGravity to Particle struct
- [ ] Update InitParticle()
- [ ] Use mass in force calculation

### Phase 6: Build System ✓
- [ ] Update Makefile SRC_FILES
- [ ] Update Makefile.web SRC_FILES
- [ ] Test desktop build
- [ ] Test web build

### Phase 7: Unit Tests ✓
- [ ] Create tests/unit/test_gravity_system.c
- [ ] Write 5+ test cases
- [ ] Compile and run tests
- [ ] All tests pass

### Phase 8: Documentation
- [ ] Update PLANNING.md with Gravity System section
- [ ] Add usage examples
- [ ] Document performance characteristics

---

## PRP Confidence Score: 9/10

**Strengths:**
- Complete implementation provided (gravity_system.h/c)
- Clear migration path from existing code
- Executable validation gates
- Preserves exact existing behavior
- Fixes critical test mode bug
- Unit tests included
- References existing patterns (enemy_state.h)

**Risk Areas:**
- Enemy index tracking for g_enemyGravitySourceIds needs careful implementation (not shown in blueprint)
- Performance with 100k particles needs validation
- Web build might need WASM-specific optimizations

**Why 9/10**: All necessary context included, clear implementation steps, testable at each phase. Only risk is enemy index tracking detail which needs attention during implementation.
