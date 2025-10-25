# Enemy Test Mode - Developer Interactive Testing System

**PRP Version**: 1.0
**Date**: 2025-10-25
**Status**: Draft
**Confidence Score**: 8/10

---

## Goal

Create an **interactive developer test mode** that allows developers to spawn, remove, and test individual enemy types in real-time without recompiling. This mode should work on both desktop and WebAssembly builds, providing a sandboxed environment for enemy behavior testing, balancing, and debugging.

## Why

- **Faster Iteration**: Test enemy behaviors without playing through entire stages
- **Balance Tuning**: Quickly adjust and observe enemy parameters
- **Visual Debugging**: See enemy AI, movement patterns, and abilities in isolation
- **Parallel Development**: Multiple developers can test different enemies independently
- **Quality Assurance**: Validate enemy behaviors before integrating into stages

## What

### User-Visible Behavior

**Desktop Build:**
1. Launch game with `./bin/game --test-mode` or `make test-enemy`
2. Clean environment appears with particles and player
3. Press number keys (1-9, 0) to select enemy type
4. Click anywhere on screen to spawn selected enemy at cursor position
5. Press 'R' to remove nearest enemy to cursor
6. Press 'C' to clear all enemies
7. Press 'F1' to toggle help overlay
8. Press 'ESC' to exit test mode

**Web Build:**
1. Select "Developer Test Mode" from dropdown on GitHub Pages
2. Same interactive controls as desktop
3. Touch support on mobile (tap to spawn, long-press to remove)

### Technical Requirements

- New game state: `GAME_STATE_TEST_MODE`
- Mouse input integration with existing input system
- Test mode stage with clean environment
- Enemy type selector UI
- Interactive spawn/remove system
- Help overlay with controls
- Works on desktop (macOS, Windows) and WebAssembly

### Success Criteria

- [ ] Can launch test mode from command line and web interface
- [ ] All 11 enemy types can be spawned interactively
- [ ] Enemy removal works with 'R' key and mouse proximity
- [ ] Clear all enemies works with 'C' key
- [ ] Help overlay displays all controls
- [ ] No memory leaks during spawn/remove cycles
- [ ] 60 FPS with 20+ enemies and 100,000 particles
- [ ] Unit tests pass for test mode initialization and enemy management
- [ ] Works on both desktop and WebAssembly builds

---

## All Needed Context

### Documentation & References

```yaml
# MUST READ - Include these in your context window

- file: src/core/game.h
  why: |
    - Game state enum (add GAME_STATE_TEST_MODE)
    - Game struct (stores current test mode state)
    - Function declarations for game loop

- file: src/core/game.c
  why: |
    - DrawGame() function - see how UI is rendered for different game states
    - UpdateGame() function - see how game states are managed
    - SpawnEnemyByType() function - already exists, reuse for test mode

- file: src/core/input_handler.c
  why: |
    - Current keyboard input handling pattern with event system
    - Memory pool usage for input events
    - How to extend with mouse input

- file: src/entities/enemy.h
  why: |
    - EnemyType enum - all 11 enemy types
    - Enemy struct - understand enemy data structure
    - MAX_ENEMIES constant - limit for spawning

- file: src/entities/managers/stages/stage_test.c (to be created)
  why: |
    - Pattern to follow from existing stage files (stage_1.c, etc.)
    - CreateStageTest() function similar to CreateStage1()

- file: src/minunit/minunit.h
  why: |
    - Unit testing framework - MU_TEST, mu_assert, etc.
    - Use for testing test mode initialization

- file: src/main.c
  why: |
    - Command-line argument parsing (see ParseStartingStage)
    - Game initialization and main loop
    - How to add --test-mode argument

- url: https://github.com/raysan5/raylib/wiki/raylib-input-system
  section: Mouse Input Functions
  why: |
    - GetMousePosition() returns Vector2
    - IsMouseButtonPressed(MOUSE_LEFT_BUTTON) for clicks
    - Works identically on desktop and web

- url: https://www.raylib.com/cheatsheet/cheatsheet.html
  section: Input Handling
  critical: |
    Mouse buttons: MOUSE_LEFT_BUTTON (0), MOUSE_RIGHT_BUTTON (1), MOUSE_MIDDLE_BUTTON (2)
    All mouse functions work in Emscripten (WebAssembly)
```

### Current Codebase Tree

```
particles/
├── src/
│   ├── core/
│   │   ├── game.c/h              # Game state management
│   │   ├── input_handler.c/h     # Keyboard input (extend for mouse)
│   │   ├── event/
│   │   │   ├── event_system.c/h  # Event publish-subscribe
│   │   │   └── event_types.h     # Event data structures
│   │   └── memory_pool.c/h       # Memory pool for events
│   ├── entities/
│   │   ├── enemy.c/h             # Enemy implementation
│   │   ├── player.c/h            # Player implementation
│   │   ├── particle.c/h          # Particle system
│   │   └── managers/
│   │       ├── enemy_manager.c/h
│   │       ├── stage_manager.c/h
│   │       └── stages/
│   │           ├── stage_common.h
│   │           ├── stage_1.c through stage_10.c
│   ├── main.c                    # Entry point, arg parsing
│   └── minunit/
│       └── minunit.h             # Testing framework
├── tests/
│   └── unit/                     # Unit tests
├── raylib-wasm/
│   └── shell.html                # Web UI with dropdown
├── Makefile                      # Desktop build
├── Makefile.web                  # WebAssembly build
├── PLANNING.md                   # Project documentation
└── TASK.md                       # Task tracking
```

### Desired Codebase Tree (Files to Add)

```
particles/
├── src/
│   ├── core/
│   │   ├── dev_test_mode.c       # Test mode logic and UI
│   │   └── dev_test_mode.h       # Test mode interface
│   └── entities/
│       └── managers/
│           └── stages/
│               └── stage_test.c  # Test mode stage configuration
├── tests/
│   └── unit/
│       └── test_dev_test_mode.c  # Unit tests for test mode
└── raylib-wasm/
    └── shell.html                # Updated with test mode option
```

**File Responsibilities:**

- **dev_test_mode.c/h**:
  - Test mode state management
  - Enemy selector UI rendering
  - Help overlay rendering
  - Mouse input handling for spawning
  - Enemy removal logic
  - Keyboard shortcut handling

- **stage_test.c**:
  - CreateStageTest() function
  - Minimal stage configuration (no waves, no win conditions)
  - Infinite time, clean environment
  - Space grey background, white particles

- **test_dev_test_mode.c**:
  - Unit tests for test mode initialization
  - Enemy selector state tests
  - Spawn/remove functionality tests

---

## Implementation Blueprint

### Phase 1: Core Test Mode Infrastructure

#### Step 1.1: Add Test Mode Game State

**File**: `src/core/game.h`

```c
// Add to GameState enum
typedef enum {
    GAME_STATE_TUTORIAL,
    GAME_STATE_PLAYING,
    GAME_STATE_OVER,
    GAME_STATE_SCORE_ENTRY,
    GAME_STATE_STAGE_INTRO,
    GAME_STATE_STAGE_COMPLETE,
    GAME_STATE_VICTORY,
    GAME_STATE_TEST_MODE        // NEW: Developer test mode
} GameState;
```

**Validation**: Compile check - `make compile-stage-1`

---

#### Step 1.2: Create Test Mode Stage

**File**: `src/entities/managers/stages/stage_test.c`

Follow pattern from existing stages (stage_1.c):

```c
/**
 * @file stage_test.c
 * @brief Developer Test Mode Stage
 *
 * Clean environment for testing individual enemy types.
 * No waves, no win conditions, infinite time.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Test Mode Stage configuration
 *
 * Stage characteristics:
 * - No enemy waves (manual spawning only)
 * - No kill targets (infinite play)
 * - Clean visual environment
 * - Space grey background, white particles
 *
 * @return Stage struct with test configuration
 */
Stage CreateStageTest(void) {
    Stage stage = {0};
    stage.stageNumber = 0;  // Special stage number for test mode
    strcpy(stage.name, "Developer Test Mode");
    strcpy(stage.description, "Interactive enemy testing environment");

    stage.waveCount = 0;  // No automatic waves
    stage.targetKills = 0;  // No win condition
    stage.maxEnemiesAlive = 50;  // Higher limit for testing

    // Standard multipliers (no modifications)
    stage.enemyHealthMultiplier = 1.0f;
    stage.enemySpeedMultiplier = 1.0f;
    stage.enemySizeMultiplier = 1.0f;

    // Consistent visual theme
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey
    stage.particleColor = WHITE;  // White particles

    stage.hasSpecialEffect = false;
    stage.particleAttractionMultiplier = 1.0f;

    return stage;
}
```

**Update**: `src/entities/managers/stages/stage_common.h`

```c
// Add declaration
Stage CreateStageTest(void);
```

**Validation**: `make compile-stage-test` (after updating Makefile)

---

#### Step 1.3: Create Test Mode Module

**File**: `src/core/dev_test_mode.h`

```c
#ifndef DEV_TEST_MODE_H
#define DEV_TEST_MODE_H

#include "raylib.h"
#include "../entities/enemy.h"

/**
 * @brief Test mode state
 */
typedef struct {
    EnemyType selectedEnemyType;  // Currently selected enemy type for spawning
    bool showHelp;                // Whether to show help overlay
    int enemiesSpawned;           // Total enemies spawned this session
    int enemiesRemoved;           // Total enemies removed this session
} TestModeState;

/**
 * @brief Initialize test mode
 * @return TestModeState Initial test mode state
 */
TestModeState InitTestMode(void);

/**
 * @brief Update test mode logic
 * @param state Test mode state
 * @param game Game instance
 */
void UpdateTestMode(TestModeState* state, void* game);  // void* to avoid circular dependency

/**
 * @brief Draw test mode UI
 * @param state Test mode state
 * @param screenWidth Screen width
 * @param screenHeight Screen height
 */
void DrawTestModeUI(TestModeState* state, int screenWidth, int screenHeight);

/**
 * @brief Handle mouse input for enemy spawning
 * @param state Test mode state
 * @param game Game instance
 */
void HandleTestModeMouseInput(TestModeState* state, void* game);

/**
 * @brief Handle keyboard shortcuts for test mode
 * @param state Test mode state
 * @param game Game instance
 */
void HandleTestModeKeyboard(TestModeState* state, void* game);

/**
 * @brief Remove nearest enemy to mouse cursor
 * @param game Game instance
 * @param mousePos Mouse position
 * @return true if enemy was removed
 */
bool RemoveNearestEnemy(void* game, Vector2 mousePos);

/**
 * @brief Get enemy type name as string
 * @param type Enemy type
 * @return const char* Enemy type name
 */
const char* GetEnemyTypeName(EnemyType type);

#endif // DEV_TEST_MODE_H
```

---

**File**: `src/core/dev_test_mode.c`

```c
#include "dev_test_mode.h"
#include "game.h"
#include <raymath.h>
#include <string.h>
#include <stdio.h>

// Enemy type names for UI display
static const char* ENEMY_TYPE_NAMES[] = {
    "BASIC",        // ENEMY_TYPE_BASIC
    "TRACKER",      // ENEMY_TYPE_TRACKER
    "SPEEDY",       // ENEMY_TYPE_SPEEDY
    "SPLITTER",     // ENEMY_TYPE_SPLITTER
    "ORBITER",      // ENEMY_TYPE_ORBITER
    "BOSS_1",       // ENEMY_TYPE_BOSS_1
    "TELEPORTER",   // ENEMY_TYPE_TELEPORTER
    "REPULSOR",     // ENEMY_TYPE_REPULSOR
    "CLUSTER",      // ENEMY_TYPE_CLUSTER
    "BOSS_FINAL",   // ENEMY_TYPE_BOSS_FINAL
    "BLACKHOLE"     // ENEMY_TYPE_BLACKHOLE
};

// Keyboard shortcuts for enemy selection (1-9, 0 for BLACKHOLE)
static const int ENEMY_SELECTION_KEYS[] = {
    KEY_ONE,    // ENEMY_TYPE_BASIC
    KEY_TWO,    // ENEMY_TYPE_TRACKER
    KEY_THREE,  // ENEMY_TYPE_SPEEDY
    KEY_FOUR,   // ENEMY_TYPE_SPLITTER
    KEY_FIVE,   // ENEMY_TYPE_ORBITER
    KEY_SIX,    // ENEMY_TYPE_BOSS_1
    KEY_SEVEN,  // ENEMY_TYPE_TELEPORTER
    KEY_EIGHT,  // ENEMY_TYPE_REPULSOR
    KEY_NINE,   // ENEMY_TYPE_CLUSTER
    KEY_ZERO,   // ENEMY_TYPE_BLACKHOLE (0 = 10th)
};

/**
 * Initialize test mode
 */
TestModeState InitTestMode(void) {
    TestModeState state = {
        .selectedEnemyType = ENEMY_TYPE_BASIC,
        .showHelp = true,  // Show help on startup
        .enemiesSpawned = 0,
        .enemiesRemoved = 0
    };
    return state;
}

/**
 * Get enemy type name as string
 */
const char* GetEnemyTypeName(EnemyType type) {
    if (type >= 0 && type < ENEMY_TYPE_COUNT) {
        return ENEMY_TYPE_NAMES[type];
    }
    return "UNKNOWN";
}

/**
 * Handle keyboard shortcuts for test mode
 */
void HandleTestModeKeyboard(TestModeState* state, void* gamePtr) {
    Game* game = (Game*)gamePtr;

    // Toggle help overlay with F1
    if (IsKeyPressed(KEY_F1)) {
        state->showHelp = !state->showHelp;
    }

    // Clear all enemies with C
    if (IsKeyPressed(KEY_C)) {
        game->enemyCount = 0;
        state->enemiesRemoved = state->enemiesSpawned;
    }

    // Remove nearest enemy with R
    if (IsKeyPressed(KEY_R)) {
        Vector2 mousePos = GetMousePosition();
        if (RemoveNearestEnemy(game, mousePos)) {
            state->enemiesRemoved++;
        }
    }

    // Enemy type selection (1-9, 0)
    for (int i = 0; i < ENEMY_TYPE_COUNT && i < 10; i++) {
        if (IsKeyPressed(ENEMY_SELECTION_KEYS[i])) {
            state->selectedEnemyType = (EnemyType)i;
        }
    }

    // Special case: 0 key selects BLACKHOLE (last enemy)
    if (IsKeyPressed(KEY_ZERO)) {
        state->selectedEnemyType = ENEMY_TYPE_BLACKHOLE;
    }
}

/**
 * Handle mouse input for enemy spawning
 */
void HandleTestModeMouseInput(TestModeState* state, void* gamePtr) {
    Game* game = (Game*)gamePtr;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();

        // Check if we can spawn more enemies
        if (game->enemyCount >= MAX_ENEMIES) {
            return;  // Max enemies reached
        }

        // Spawn enemy at mouse position
        Enemy newEnemy = InitEnemyByType(
            state->selectedEnemyType,
            game->screenWidth,
            game->screenHeight,
            game->player.position
        );

        // Override spawn position with mouse position
        newEnemy.position = mousePos;

        // Add to game's enemy array
        game->enemies[game->enemyCount] = newEnemy;
        game->enemyCount++;
        state->enemiesSpawned++;
    }
}

/**
 * Remove nearest enemy to mouse cursor
 */
bool RemoveNearestEnemy(void* gamePtr, Vector2 mousePos) {
    Game* game = (Game*)gamePtr;

    if (game->enemyCount == 0) return false;

    // Find nearest enemy
    int nearestIndex = -1;
    float nearestDistance = 999999.0f;

    for (int i = 0; i < game->enemyCount; i++) {
        float distance = Vector2Distance(game->enemies[i].position, mousePos);
        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestIndex = i;
        }
    }

    if (nearestIndex >= 0) {
        // Remove enemy by shifting array
        for (int i = nearestIndex; i < game->enemyCount - 1; i++) {
            game->enemies[i] = game->enemies[i + 1];
        }
        game->enemyCount--;
        return true;
    }

    return false;
}

/**
 * Update test mode logic
 */
void UpdateTestMode(TestModeState* state, void* gamePtr) {
    HandleTestModeKeyboard(state, gamePtr);
    HandleTestModeMouseInput(state, gamePtr);
}

/**
 * Draw test mode UI
 */
void DrawTestModeUI(TestModeState* state, int screenWidth, int screenHeight) {
    // Draw enemy selector panel (top-left)
    int panelX = 10;
    int panelY = 100;
    int panelWidth = 250;
    int panelHeight = 80;

    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, YELLOW);

    // Selected enemy type
    DrawText("SELECTED ENEMY:", panelX + 10, panelY + 10, 16, WHITE);
    const char* typeName = GetEnemyTypeName(state->selectedEnemyType);
    DrawText(typeName, panelX + 10, panelY + 30, 20, YELLOW);

    // Stats
    char statsText[128];
    sprintf(statsText, "Spawned: %d  Removed: %d", state->enemiesSpawned, state->enemiesRemoved);
    DrawText(statsText, panelX + 10, panelY + 55, 14, LIGHTGRAY);

    // Draw help overlay if enabled
    if (state->showHelp) {
        int helpX = screenWidth - 320;
        int helpY = 10;
        int helpWidth = 310;
        int helpHeight = 260;

        DrawRectangle(helpX, helpY, helpWidth, helpHeight, Fade(BLACK, 0.8f));
        DrawRectangleLines(helpX, helpY, helpWidth, helpHeight, GREEN);

        DrawText("TEST MODE CONTROLS", helpX + 10, helpY + 10, 16, GREEN);
        DrawText("F1: Toggle Help", helpX + 10, helpY + 35, 14, WHITE);
        DrawText("1-9: Select Enemy Type", helpX + 10, helpY + 55, 14, WHITE);
        DrawText("0: Select BLACKHOLE", helpX + 10, helpY + 75, 14, WHITE);
        DrawText("Left Click: Spawn Enemy", helpX + 10, helpY + 95, 14, WHITE);
        DrawText("R: Remove Nearest Enemy", helpX + 10, helpY + 115, 14, WHITE);
        DrawText("C: Clear All Enemies", helpX + 10, helpY + 135, 14, WHITE);
        DrawText("ESC: Exit Test Mode", helpX + 10, helpY + 155, 14, WHITE);
        DrawText("", helpX + 10, helpY + 175, 14, WHITE);
        DrawText("ENEMY TYPES:", helpX + 10, helpY + 195, 14, YELLOW);
        DrawText("1=BASIC  2=TRACKER  3=SPEEDY", helpX + 10, helpY + 215, 12, LIGHTGRAY);
        DrawText("4=SPLIT  5=ORBIT   6=BOSS", helpX + 10, helpY + 230, 12, LIGHTGRAY);
        DrawText("7=TELE   8=REPULSE 9=CLUSTER", helpX + 10, helpY + 245, 12, LIGHTGRAY);
    }
}
```

**Validation**:
```bash
# Compile check
make compile-dev-test-mode  # After adding to Makefile
```

---

### Phase 2: Game Loop Integration

#### Step 2.1: Update Main Game Loop

**File**: `src/main.c`

```c
// Add include
#include "core/dev_test_mode.h"

/**
 * Parse command line arguments for test mode
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return true if test mode requested
 */
bool ParseTestMode(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test-mode") == 0) {
            return true;
        }
    }
    return false;
}

// In main():
int main(int argc, char *argv[]) {
    // ... existing initialization ...

    // Parse command-line arguments
    int startingStage = ParseStartingStage(argc, argv);
    bool testMode = ParseTestMode(argc, argv);

    Game game = InitGame(screenWidth, screenHeight);

    // Jump to test mode if requested
    if (testMode) {
        game.currentStageNumber = 0;
        game.gameState = GAME_STATE_TEST_MODE;
        game.currentStage = CreateStageTest();
        game.currentStage.state = STAGE_STATE_ACTIVE;

        // Initialize test mode state
        TestModeState testModeState = InitTestMode();

        // Store in game struct (need to add to Game struct)
        // game.testModeState = testModeState;  // See Step 2.2
    }

    // ... rest of main loop ...
}
```

---

#### Step 2.2: Update Game Struct

**File**: `src/core/game.h`

```c
#include "dev_test_mode.h"  // Add at top

typedef struct Game {
    // ... existing fields ...

    // Test mode
    TestModeState testModeState;  // Test mode state

} Game;
```

---

#### Step 2.3: Update Game Update Loop

**File**: `src/core/game.c`

```c
#include "dev_test_mode.h"  // Add at top

void UpdateGame(Game* game) {
    game->deltaTime = GetFrameTime();

    // ... existing state handling ...

    // Test mode update
    if (game->gameState == GAME_STATE_TEST_MODE) {
        // Update test mode logic
        UpdateTestMode(&game->testModeState, game);

        // Update player
        UpdatePlayer(&game->player, game->screenWidth, game->screenHeight, game->deltaTime);

        // Update particles
        UpdateAllParticles(game, IsKeyDown(KEY_SPACE));

        // Update enemies
        UpdateAllEnemies(game);

        // Handle collisions
        ProcessEnemyCollisions(game);

        // Exit test mode with ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->gameState = GAME_STATE_TUTORIAL;
        }

        return;
    }

    // ... existing game state logic ...
}
```

---

#### Step 2.4: Update Game Draw Loop

**File**: `src/core/game.c`

```c
void DrawGame(Game* game) {
    BeginDrawing();

    // Set background color from current stage
    ClearBackground(game->currentStage.backgroundColor);

    // ... existing state rendering ...

    // Test mode rendering
    if (game->gameState == GAME_STATE_TEST_MODE) {
        // Draw particles
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            DrawParticlePixel(game->particles[i]);
        }

        // Draw enemies
        for (int i = 0; i < game->enemyCount; i++) {
            DrawEnemy(game->enemies[i]);
        }

        // Draw player
        if (!game->player.isInvincible || ((int)(GetTime() * 10) % 2 == 0)) {
            DrawRectangle(game->player.position.x, game->player.position.y,
                         game->player.size, game->player.size, RED);
        }

        // Draw test mode UI
        DrawTestModeUI(&game->testModeState, game->screenWidth, game->screenHeight);

        // Draw FPS
        DrawFPS(10, 10);

        EndDrawing();
        return;
    }

    // ... existing rendering code ...
}
```

**Validation**:
```bash
# Compile and run
make clean && make
./bin/game --test-mode
# Should show test mode environment with UI
```

---

### Phase 3: Web Build Integration

#### Step 3.1: Update Web Makefile

**File**: `Makefile.web`

```makefile
# Add dev_test_mode.c to SRC_FILES
SRC_FILES := \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/core/game.c \
    $(SRC_DIR)/core/dev_test_mode.c \
    # ... rest of files ...
    $(STAGES_DIR)/stage_test.c
```

**Validation**:
```bash
# Compile web build
make -f Makefile.web clean
make -f Makefile.web
# Check that particle_storm.html and .wasm/.js files are generated
```

---

#### Step 3.2: Update Web UI

**File**: `raylib-wasm/shell.html`

Update the stage selector dropdown to include test mode:

```html
<!-- Find the stage selector dropdown and add test mode option -->
<select id="stageSelector" onchange="changeStage()">
    <option value="0">Full Game (All Stages)</option>
    <option value="test">Developer Test Mode</option>  <!-- NEW -->
    <option value="1">Stage 1: Gravitational Anomaly</option>
    <option value="2">Stage 2: The Hunt Begins</option>
    <!-- ... existing stages ... -->
</select>

<script>
function changeStage() {
    var stageSelector = document.getElementById('stageSelector');
    var selectedValue = stageSelector.value;
    var currentStage = selectedValue;

    // Handle test mode
    if (selectedValue === 'test') {
        // Reload page with test mode parameter
        window.location.href = window.location.pathname + '?test-mode=1';
        return;
    }

    // ... existing stage selection logic ...
}

// Get test mode from URL parameter
function getTestModeFromURL() {
    var urlParams = new URLSearchParams(window.location.search);
    return urlParams.get('test-mode') === '1';
}

var Module = {
    arguments: [],
    // ... existing Module configuration ...
};

// Set up arguments based on URL
var isTestMode = getTestModeFromURL();
if (isTestMode) {
    Module.arguments = ['--test-mode'];
} else {
    var currentStage = getStageFromURL();
    if (currentStage !== '0') {
        Module.arguments = ['--start-stage', currentStage];
    }
}
</script>
```

**Validation**:
```bash
# Serve locally
python3 -m http.server 8000
# Open http://localhost:8000/particle_storm.html?test-mode=1
# Should load test mode with interactive controls
```

---

### Phase 4: Build System Updates

#### Step 4.1: Update Desktop Makefile

**File**: `Makefile`

```makefile
# Add dev_test_mode to object files
CORE_DIR := $(SRC_DIR)/core
DEV_TEST_MODE_OBJ := $(CORE_DIR)/dev_test_mode.o

# Add to OBJS
OBJS := $(MAIN_OBJ) $(GAME_OBJ) $(PHYSICS_OBJ) $(INPUT_HANDLER_OBJ) \
        $(MEMORY_POOL_OBJ) $(EVENT_SYSTEM_OBJ) $(PLAYER_OBJ) \
        $(PARTICLE_OBJ) $(ENEMY_OBJ) $(EXPLOSION_OBJ) $(ITEM_MANAGER_OBJ) \
        $(HPPOTION_OBJ) $(STAR_ITEM_OBJ) $(STAGE_MANAGER_OBJ) \
        $(DEV_TEST_MODE_OBJ) \  # NEW
        $(STAGE_OBJS)

# Add compilation rule
$(DEV_TEST_MODE_OBJ): $(CORE_DIR)/dev_test_mode.c $(CORE_DIR)/dev_test_mode.h
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Add convenience target
test-enemy: all
	@echo "Launching developer test mode..."
	@./$(BIN_DIR)/game --test-mode

.PHONY: test-enemy
```

**Validation**:
```bash
make test-enemy
# Should compile and launch test mode
```

---

### Phase 5: Unit Testing

#### Step 5.1: Create Unit Tests

**File**: `tests/unit/test_dev_test_mode.c`

```c
#include "../../src/minunit/minunit.h"
#include "../../src/core/dev_test_mode.h"
#include "../../src/core/game.h"

/**
 * Test: Initialize test mode
 */
MU_TEST(test_init_test_mode) {
    TestModeState state = InitTestMode();

    mu_assert_int_eq(ENEMY_TYPE_BASIC, state.selectedEnemyType);
    mu_assert(state.showHelp == true, "Help should show on init");
    mu_assert_int_eq(0, state.enemiesSpawned);
    mu_assert_int_eq(0, state.enemiesRemoved);
}

/**
 * Test: Get enemy type name
 */
MU_TEST(test_get_enemy_type_name) {
    const char* name = GetEnemyTypeName(ENEMY_TYPE_BASIC);
    mu_assert_string_eq("BASIC", name);

    name = GetEnemyTypeName(ENEMY_TYPE_TRACKER);
    mu_assert_string_eq("TRACKER", name);

    name = GetEnemyTypeName(ENEMY_TYPE_BLACKHOLE);
    mu_assert_string_eq("BLACKHOLE", name);
}

/**
 * Test: Remove nearest enemy
 */
MU_TEST(test_remove_nearest_enemy) {
    // Create a minimal game state
    Game game = {
        .screenWidth = 800,
        .screenHeight = 800,
        .enemyCount = 0
    };

    // Allocate enemy array
    game.enemies = (Enemy*)malloc(sizeof(Enemy) * MAX_ENEMIES);

    // Add test enemy at (100, 100)
    game.enemies[0].position = (Vector2){100, 100};
    game.enemyCount = 1;

    // Remove enemy near (105, 105)
    bool removed = RemoveNearestEnemy(&game, (Vector2){105, 105});

    mu_assert(removed, "Should remove enemy");
    mu_assert_int_eq(0, game.enemyCount);

    // Cleanup
    free(game.enemies);
}

/**
 * Test: Don't remove when no enemies
 */
MU_TEST(test_remove_no_enemies) {
    Game game = {
        .screenWidth = 800,
        .screenHeight = 800,
        .enemyCount = 0
    };

    game.enemies = (Enemy*)malloc(sizeof(Enemy) * MAX_ENEMIES);

    bool removed = RemoveNearestEnemy(&game, (Vector2){100, 100});

    mu_assert(!removed, "Should not remove when no enemies");
    mu_assert_int_eq(0, game.enemyCount);

    free(game.enemies);
}

/**
 * Test suite
 */
MU_TEST_SUITE(dev_test_mode_suite) {
    MU_RUN_TEST(test_init_test_mode);
    MU_RUN_TEST(test_get_enemy_type_name);
    MU_RUN_TEST(test_remove_nearest_enemy);
    MU_RUN_TEST(test_remove_no_enemies);
}

/**
 * Main test runner
 */
int main() {
    MU_RUN_SUITE(dev_test_mode_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

**Validation**:
```bash
# Compile and run test
gcc -o tests/unit/test_dev_test_mode_compiled \
    tests/unit/test_dev_test_mode.c \
    src/core/dev_test_mode.c \
    -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib \
    -lraylib -lm

./tests/unit/test_dev_test_mode_compiled
# Should print: "4 tests, N assertions, 0 failures"
```

---

### Phase 6: Documentation Updates

#### Step 6.1: Update PLANNING.md

**File**: `PLANNING.md`

Add section under "Build Commands":

```markdown
# Test developer test mode (interactive enemy spawning)
make test-enemy
./bin/game --test-mode

# Web version
https://namyunwoo.github.io/particles/?test-mode=1
```

Add section under "Developer Tools":

```markdown
### Developer Test Mode

Interactive environment for testing enemy behaviors in isolation.

**Controls:**
- Number keys 1-9, 0: Select enemy type
- Left Click: Spawn enemy at cursor
- R: Remove nearest enemy
- C: Clear all enemies
- F1: Toggle help overlay
- ESC: Exit test mode

**Usage:**
```bash
# Desktop
./bin/game --test-mode
make test-enemy

# Web (GitHub Pages)
https://namyunwoo.github.io/particles/?test-mode=1
```

**Features:**
- Spawn any of 11 enemy types instantly
- Remove enemies individually or clear all
- No stage progression or win conditions
- Clean testing environment
- Works on desktop and web builds
```

**Validation**: Read PLANNING.md to ensure formatting is correct

---

#### Step 6.2: Update TASK.md

**File**: `TASK.md`

Add completed task:

```markdown
## Implement Developer Test Mode ✓ [COMPLETED 2025-10-25]
- Interactive enemy spawning/removal system
- Mouse input integration
- Keyboard shortcuts for enemy selection (1-9, 0)
- Test mode stage with clean environment
- Help overlay with F1 toggle
- Desktop and WebAssembly support
- Unit tests for test mode functionality
- GitHub Pages dropdown integration

**Features:**
- Spawn enemies with left click at cursor position
- Remove nearest enemy with R key
- Clear all enemies with C key
- Select from 11 enemy types with number keys
- Real-time spawn/remove statistics
- Works on both desktop and web builds

**Controls:**
```bash
make test-enemy                    # Launch test mode (desktop)
./bin/game --test-mode            # Direct launch
https://.../?test-mode=1          # Web version
```

**Benefits:**
- Faster enemy behavior testing
- No need to replay stages for specific enemies
- Visual debugging of AI and movement patterns
- Parallel development and testing
- Professional developer tools
```

---

### Phase 7: Validation Gates

#### Validation Gate 1: Compilation

```bash
# Desktop build
make clean
make
# Expected: No warnings, no errors

# Web build
make -f Makefile.web clean
make -f Makefile.web
# Expected: particle_storm.html, .js, .wasm files generated
```

---

#### Validation Gate 2: Unit Tests

```bash
# Run test mode unit tests
make test-dev-test-mode  # After adding Makefile target
# Expected: "4 tests, N assertions, 0 failures"
```

---

#### Validation Gate 3: Functional Testing

**Desktop:**
```bash
./bin/game --test-mode
# Expected behaviors:
# - Test mode UI appears
# - Press 1: Selects BASIC enemy
# - Click mouse: Spawns BASIC at cursor
# - Press 2: Selects TRACKER enemy
# - Click mouse: Spawns TRACKER at cursor
# - Press R: Removes nearest enemy
# - Press C: Clears all enemies
# - Press F1: Toggles help overlay
# - Press ESC: Exits to tutorial
```

**Web:**
```bash
python3 -m http.server 8000
# Open: http://localhost:8000/particle_storm.html?test-mode=1
# Test same behaviors as desktop
```

---

#### Validation Gate 4: Performance

```bash
./bin/game --test-mode
# Spawn 20+ enemies by clicking
# Expected: 60 FPS maintained
# Check FPS counter in top-left
```

---

#### Validation Gate 5: Memory Safety

```bash
# Run with address sanitizer
make clean
make CFLAGS="-fsanitize=address -g"
./bin/game --test-mode
# Spawn and remove enemies 50+ times
# Press C to clear all
# Expected: No memory leaks reported
```

---

## Anti-Patterns to Avoid

- ❌ Don't use malloc/free for spawning - reuse existing SpawnEnemyByType()
- ❌ Don't skip mouse input validation - check MOUSE_LEFT_BUTTON constant
- ❌ Don't hardcode enemy type names - use array lookup
- ❌ Don't forget to update Makefile.web for WebAssembly build
- ❌ Don't modify existing game states - add new GAME_STATE_TEST_MODE
- ❌ Don't create files > 500 lines - keep modules focused

---

## Task Checklist (Implementation Order)

```markdown
Phase 1: Core Infrastructure
- [ ] Add GAME_STATE_TEST_MODE to game.h
- [ ] Create src/entities/managers/stages/stage_test.c
- [ ] Update stages/stage_common.h with CreateStageTest()
- [ ] Create src/core/dev_test_mode.h
- [ ] Create src/core/dev_test_mode.c
- [ ] Compile and verify no errors

Phase 2: Game Loop Integration
- [ ] Update src/main.c with ParseTestMode()
- [ ] Add testModeState to Game struct in game.h
- [ ] Update UpdateGame() in game.c for test mode
- [ ] Update DrawGame() in game.c for test mode rendering
- [ ] Test: Launch with --test-mode flag

Phase 3: Web Build
- [ ] Update Makefile.web with dev_test_mode.c
- [ ] Update raylib-wasm/shell.html dropdown
- [ ] Add URL parameter parsing for ?test-mode=1
- [ ] Build and test web version locally

Phase 4: Build System
- [ ] Update Makefile with dev_test_mode.o
- [ ] Add make test-enemy target
- [ ] Test: make test-enemy works

Phase 5: Testing
- [ ] Create tests/unit/test_dev_test_mode.c
- [ ] Run unit tests and verify pass
- [ ] Functional testing (desktop)
- [ ] Functional testing (web)
- [ ] Performance testing (60 FPS with 20+ enemies)
- [ ] Memory leak testing (address sanitizer)

Phase 6: Documentation
- [ ] Update PLANNING.md with test mode docs
- [ ] Update TASK.md with completion entry
- [ ] Update WEB_BUILD_GUIDE.md with test mode URL

Phase 7: Validation
- [ ] All validation gates pass
- [ ] No compiler warnings
- [ ] Unit tests pass
- [ ] Works on desktop (macOS)
- [ ] Works on WebAssembly
- [ ] 60 FPS maintained
- [ ] No memory leaks
```

---

## Confidence Score: 8/10

**Strengths:**
- ✅ Comprehensive context from codebase analysis
- ✅ Follows existing patterns (stage system, input handling)
- ✅ Reuses existing functions (SpawnEnemyByType, InitEnemyByType)
- ✅ Clear validation gates with executable tests
- ✅ Both desktop and web builds addressed
- ✅ Memory pool awareness (no malloc/free in hot paths)
- ✅ Unit tests with MinUnit framework

**Risks:**
- ⚠️ Game struct circular dependency (dev_test_mode.h includes game.h) - mitigated with void* pointers
- ⚠️ Mouse input not currently in event system - direct raylib calls acceptable
- ⚠️ WebAssembly mouse input untested - raylib docs confirm compatibility

**Mitigation:**
- Use void* pointers in dev_test_mode.h to avoid circular includes
- Test web build early to catch Emscripten issues
- Follow raylib examples for mouse input (confirmed cross-platform)

---

## Summary

This PRP provides a complete implementation path for a developer test mode with interactive enemy spawning and removal. The implementation follows project conventions, reuses existing systems, and includes comprehensive validation. Expected implementation time: 2-3 hours with testing.

The system enables rapid iteration on enemy behaviors without recompilation, working identically on desktop and web builds. All validation gates are executable, ensuring quality and correctness.
