# PRP: Star Item with Invincibility Feature

## Feature Requirements
Implement a Star item that grants the player 20 seconds of invincibility when collected. The Star spawns off-screen with a trajectory, moves through the play area, and triggers special visual/audio effects.

## Context and References

### Existing Patterns
- **Item Implementation**: `src/entities/items/hp_potion.h/c` - static item structure
- **Item Manager**: `src/entities/managers/item_manager.c` - spawn timing, collision
- **Player State**: `src/entities/player.h/c` - existing invincibility system (1.5s)
- **Visual Effects**: HP potion blinking pattern, player invincibility flash
- **Event System**: `src/core/event/event_types.h` - event publishing patterns

### Key Implementation Differences
1. **Movement**: Star moves with trajectory (unlike static HP potion)
2. **Spawn Location**: Off-screen spawn (new pattern)
3. **Visual Effect**: Rainbow color cycling (new pattern)
4. **Audio**: Background music change (new system)
5. **Duration**: 20s invincibility (vs current 1.5s)

### External Documentation
- Raylib Audio: https://www.raylib.com/cheatsheet/cheatsheet.html#audio
- Raylib Colors: https://github.com/raysan5/raylib/wiki/Working-with-Colors
- Trajectory Math: Basic 2D vector movement with constant velocity

## Implementation Blueprint

### Phase 1: Create Star Item Entity

**File: `src/entities/items/star_item.h`**
```c
#ifndef STAR_ITEM_H
#define STAR_ITEM_H

#include "raylib.h"
#include <stdbool.h>

#define STAR_BASE_SIZE 12.0f
#define STAR_SPEED 150.0f
#define STAR_INVINCIBILITY_DURATION 20.0f
#define STAR_SPAWN_MIN_INTERVAL 30.0f
#define STAR_SPAWN_MAX_INTERVAL 40.0f
#define STAR_TARGET_AREA_RADIUS 30.0f

typedef struct {
    Vector2 position;
    Vector2 velocity;  // For trajectory movement
    float radius;
    bool isActive;
    float colorTimer;  // For rainbow effect
} StarItem;

// Core functions
StarItem InitStarItem(void);
void SpawnStarItem(StarItem* star, int screenWidth, int screenHeight);
void UpdateStarItem(StarItem* star, float deltaTime, int screenWidth, int screenHeight);
void DrawStarItem(StarItem star);
bool CheckStarPlayerCollision(StarItem* star, Vector2 playerPos, float playerRadius);

// Helper functions
Vector2 CalculateStarSpawnPosition(int screenWidth, int screenHeight);
Vector2 CalculateStarVelocity(Vector2 spawnPos, int screenWidth, int screenHeight);
Color GetRainbowColor(float timer);

#endif
```

**File: `src/entities/items/star_item.c`**
```c
#include "star_item.h"
#include "../../core/memory_pool.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include <math.h>
#include <stdlib.h>

extern MemoryPool g_itemEventPool;

StarItem InitStarItem(void) {
    StarItem star = {
        .position = (Vector2){0, 0},
        .velocity = (Vector2){0, 0},
        .radius = STAR_BASE_SIZE,
        .isActive = false,
        .colorTimer = 0.0f
    };
    return star;
}

Vector2 CalculateStarSpawnPosition(int screenWidth, int screenHeight) {
    // Spawn from random edge
    int edge = rand() % 4; // 0=top, 1=right, 2=bottom, 3=left
    Vector2 pos;
    float margin = STAR_BASE_SIZE * 2;
    
    switch(edge) {
        case 0: // Top
            pos.x = margin + (rand() % (int)(screenWidth - 2 * margin));
            pos.y = -margin;
            break;
        case 1: // Right
            pos.x = screenWidth + margin;
            pos.y = margin + (rand() % (int)(screenHeight - 2 * margin));
            break;
        case 2: // Bottom
            pos.x = margin + (rand() % (int)(screenWidth - 2 * margin));
            pos.y = screenHeight + margin;
            break;
        case 3: // Left
            pos.x = -margin;
            pos.y = margin + (rand() % (int)(screenHeight - 2 * margin));
            break;
    }
    return pos;
}

Vector2 CalculateStarVelocity(Vector2 spawnPos, int screenWidth, int screenHeight) {
    // Target random point in center area
    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    float targetX = centerX + (rand() % 61 - 30); // -30 to +30
    float targetY = centerY + (rand() % 61 - 30); // -30 to +30
    
    // Calculate direction vector
    Vector2 direction = {
        targetX - spawnPos.x,
        targetY - spawnPos.y
    };
    
    // Normalize and apply speed
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x = (direction.x / length) * STAR_SPEED;
        direction.y = (direction.y / length) * STAR_SPEED;
    }
    
    return direction;
}

void SpawnStarItem(StarItem* star, int screenWidth, int screenHeight) {
    if (star->isActive) return;
    
    star->position = CalculateStarSpawnPosition(screenWidth, screenHeight);
    star->velocity = CalculateStarVelocity(star->position, screenWidth, screenHeight);
    star->isActive = true;
    star->colorTimer = 0.0f;
    
    // Publish spawn event
    if (g_itemEventPool.blocks != NULL) {
        ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
        if (data) {
            data->itemType = 1; // Star Item
            data->position = star->position;
            data->itemPtr = star;
            PublishEvent(EVENT_ITEM_SPAWNED, data);
        }
    }
}

void UpdateStarItem(StarItem* star, float deltaTime, int screenWidth, int screenHeight) {
    if (!star->isActive) return;
    
    // Update position
    star->position.x += star->velocity.x * deltaTime;
    star->position.y += star->velocity.y * deltaTime;
    
    // Update color timer for rainbow effect
    star->colorTimer += deltaTime * 2.0f; // Speed of color change
    
    // Check if out of bounds
    float margin = STAR_BASE_SIZE * 3;
    if (star->position.x < -margin || star->position.x > screenWidth + margin ||
        star->position.y < -margin || star->position.y > screenHeight + margin) {
        star->isActive = false;
        
        // Publish expire event
        if (g_itemEventPool.blocks != NULL) {
            ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
            if (data) {
                data->itemType = 1;
                data->position = star->position;
                data->itemPtr = star;
                PublishEvent(EVENT_ITEM_EXPIRED, data);
            }
        }
    }
}

Color GetRainbowColor(float timer) {
    // Create smooth rainbow transition
    float hue = fmodf(timer, 1.0f) * 360.0f;
    
    // Convert HSV to RGB (simplified)
    float h = hue / 60.0f;
    float x = 1.0f - fabsf(fmodf(h, 2.0f) - 1.0f);
    
    Color color;
    if (h < 1) { color = (Color){255, (unsigned char)(255 * x), 0, 255}; }
    else if (h < 2) { color = (Color){(unsigned char)(255 * x), 255, 0, 255}; }
    else if (h < 3) { color = (Color){0, 255, (unsigned char)(255 * x), 255}; }
    else if (h < 4) { color = (Color){0, (unsigned char)(255 * x), 255, 255}; }
    else if (h < 5) { color = (Color){(unsigned char)(255 * x), 0, 255, 255}; }
    else { color = (Color){255, 0, (unsigned char)(255 * x), 255}; }
    
    return color;
}

void DrawStarItem(StarItem star) {
    if (!star.isActive) return;
    
    Color starColor = GetRainbowColor(star.colorTimer);
    
    // Draw star shape (simplified as octagon for now)
    DrawCircle((int)star.position.x, (int)star.position.y, star.radius, starColor);
    
    // Add sparkle effect
    float sparkleRadius = star.radius * 0.3f;
    Color sparkleColor = (Color){255, 255, 255, 200};
    DrawCircle((int)(star.position.x - star.radius * 0.5f), 
               (int)(star.position.y - star.radius * 0.5f), 
               sparkleRadius, sparkleColor);
}

bool CheckStarPlayerCollision(StarItem* star, Vector2 playerPos, float playerRadius) {
    if (!star->isActive) return false;
    
    float dx = star->position.x - playerPos.x;
    float dy = star->position.y - playerPos.y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    return distance < (star->radius + playerRadius);
}
```

### Phase 2: Extend Item Manager

**Modify `src/entities/managers/item_manager.h`**:
```c
// Add to ItemManager struct
typedef struct {
    HPPotion hpPotion;
    StarItem starItem;  // NEW
    float potionSpawnTimer;
    float starSpawnTimer;  // NEW
    float nextStarSpawnInterval;  // NEW - random interval
    bool initialized;
} ItemManager;
```

**Modify `src/entities/managers/item_manager.c`**:
```c
// In InitItemManager()
g_itemManager.starItem = InitStarItem();
g_itemManager.starSpawnTimer = 0.0f;
g_itemManager.nextStarSpawnInterval = STAR_SPAWN_MIN_INTERVAL + 
    (rand() % (int)((STAR_SPAWN_MAX_INTERVAL - STAR_SPAWN_MIN_INTERVAL) * 100)) / 100.0f;

// In UpdateItemManager()
// Update star spawn timer
g_itemManager.starSpawnTimer += deltaTime;

// Check if should spawn star
if (g_itemManager.starSpawnTimer >= g_itemManager.nextStarSpawnInterval && 
    !g_itemManager.starItem.isActive) {
    SpawnStarItem(&g_itemManager.starItem, screenWidth, screenHeight);
    g_itemManager.starSpawnTimer = 0.0f;
    // Set next random interval
    g_itemManager.nextStarSpawnInterval = STAR_SPAWN_MIN_INTERVAL + 
        (rand() % (int)((STAR_SPAWN_MAX_INTERVAL - STAR_SPAWN_MIN_INTERVAL) * 100)) / 100.0f;
}

// Update star item
UpdateStarItem(&g_itemManager.starItem, deltaTime, screenWidth, screenHeight);

// In DrawItems()
DrawStarItem(g_itemManager.starItem);

// In CheckItemCollisions()
// Check star collision
if (CheckStarPlayerCollision(&g_itemManager.starItem, player->position, player->size)) {
    // Make player invincible
    player->isInvincible = true;
    player->invincibleTimer = STAR_INVINCIBILITY_DURATION;
    
    // Destroy star
    g_itemManager.starItem.isActive = false;
    
    // Publish collected event
    ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
    if (data) {
        data->itemType = 1; // Star
        data->position = g_itemManager.starItem.position;
        data->itemPtr = &g_itemManager.starItem;
        PublishEvent(EVENT_ITEM_COLLECTED, data);
    }
}
```

### Phase 3: Enhanced Player Invincibility Visuals

**Modify `src/entities/player.h`**:
```c
// Add to Player struct
typedef struct {
    // ... existing fields ...
    bool isStarInvincible;  // NEW - distinguish star invincibility
    float rainbowTimer;     // NEW - for rainbow effect
} Player;
```

**Modify `src/entities/player.c`**:
```c
// In UpdatePlayer() - add rainbow timer update
if (player->isInvincible && player->invincibleTimer > 18.0f) {
    // Star invincibility (first 18 seconds of 20s duration)
    player->isStarInvincible = true;
    player->rainbowTimer += deltaTime * 3.0f; // Rainbow cycle speed
} else {
    player->isStarInvincible = false;
}
```

**Modify player drawing in `src/core/game.c`**:
```c
// In DrawGame() - enhance player drawing
if (game->player.isStarInvincible) {
    // Rainbow flash effect
    Color rainbowColor = GetRainbowColor(game->player.rainbowTimer);
    // Flash between rainbow and white
    if ((int)(GetTime() * 20) % 2 == 0) {
        DrawRectangle((int)(game->player.position.x - game->player.size),
                      (int)(game->player.position.y - game->player.size),
                      (int)(game->player.size * 2), 
                      (int)(game->player.size * 2),
                      rainbowColor);
    } else {
        DrawRectangle((int)(game->player.position.x - game->player.size),
                      (int)(game->player.position.y - game->player.size),
                      (int)(game->player.size * 2), 
                      (int)(game->player.size * 2),
                      WHITE);
    }
} else if (!game->player.isInvincible || ((int)(GetTime() * 10) % 2 == 0)) {
    // Normal or damage invincibility blink
    DrawRectangle((int)(game->player.position.x - game->player.size),
                  (int)(game->player.position.y - game->player.size),
                  (int)(game->player.size * 2), 
                  (int)(game->player.size * 2),
                  WHITE);
}
```

### Phase 4: Audio System (Optional - Basic Implementation)

**Create `src/core/audio_manager.h`**:
```c
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Music backgroundMusic;
    Music starTheme;
    bool initialized;
    bool isStarThemeActive;
} AudioManager;

extern AudioManager g_audioManager;

void InitAudioManager(void);
void CleanupAudioManager(void);
void UpdateAudioManager(void);
void PlayStarTheme(void);
void StopStarTheme(void);

#endif
```

**Create `src/core/audio_manager.c`**:
```c
#include "audio_manager.h"

AudioManager g_audioManager = {0};

void InitAudioManager(void) {
    InitAudioDevice();
    
    // Note: Audio files need to be added to the project
    // For now, this is a placeholder structure
    // g_audioManager.backgroundMusic = LoadMusicStream("assets/audio/background.ogg");
    // g_audioManager.starTheme = LoadMusicStream("assets/audio/star_theme.ogg");
    
    g_audioManager.initialized = true;
    g_audioManager.isStarThemeActive = false;
}

void CleanupAudioManager(void) {
    if (!g_audioManager.initialized) return;
    
    // UnloadMusicStream(g_audioManager.backgroundMusic);
    // UnloadMusicStream(g_audioManager.starTheme);
    
    CloseAudioDevice();
    g_audioManager.initialized = false;
}

void UpdateAudioManager(void) {
    if (!g_audioManager.initialized) return;
    
    // Update music streams
    // if (g_audioManager.isStarThemeActive) {
    //     UpdateMusicStream(g_audioManager.starTheme);
    // } else {
    //     UpdateMusicStream(g_audioManager.backgroundMusic);
    // }
}

void PlayStarTheme(void) {
    if (!g_audioManager.initialized) return;
    
    // StopMusicStream(g_audioManager.backgroundMusic);
    // PlayMusicStream(g_audioManager.starTheme);
    g_audioManager.isStarThemeActive = true;
}

void StopStarTheme(void) {
    if (!g_audioManager.initialized) return;
    
    // StopMusicStream(g_audioManager.starTheme);
    // PlayMusicStream(g_audioManager.backgroundMusic);
    g_audioManager.isStarThemeActive = false;
}
```

## Unit Tests

**File: `tests/unit/test_star_item.c`**
```c
#include "../../src/minunit/minunit.h"
#include "../../src/entities/items/star_item.h"
#include "../../src/entities/managers/item_manager.h"
#include "../../src/entities/player.h"
#include "../../src/core/event/event_system.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

static Player testPlayer;

void test_setup(void) {
    srand(42); // Fixed seed for reproducible tests
    InitEventSystem();
    InitItemManager();
    testPlayer = InitPlayer(400, 300);
}

void test_teardown(void) {
    CleanupItemManager();
    CleanupEventSystem();
}

MU_TEST(test_star_init) {
    StarItem star = InitStarItem();
    mu_assert(!star.isActive, "Star should start inactive");
    mu_assert_double_eq(STAR_BASE_SIZE, star.radius);
    mu_assert_double_eq(0.0f, star.colorTimer);
}

MU_TEST(test_star_spawn_position) {
    StarItem star = InitStarItem();
    SpawnStarItem(&star, 800, 600);
    
    mu_assert(star.isActive, "Star should be active after spawn");
    
    // Check spawn is off-screen
    float margin = STAR_BASE_SIZE * 2;
    bool offScreen = (star.position.x < -margin || star.position.x > 800 + margin ||
                      star.position.y < -margin || star.position.y > 600 + margin);
    mu_assert(offScreen, "Star should spawn off-screen");
    
    // Check velocity is non-zero
    mu_assert(star.velocity.x != 0 || star.velocity.y != 0, "Star should have velocity");
    
    // Check velocity magnitude
    float speed = sqrtf(star.velocity.x * star.velocity.x + star.velocity.y * star.velocity.y);
    mu_assert_double_eq(STAR_SPEED, speed);
}

MU_TEST(test_star_movement) {
    StarItem star = InitStarItem();
    star.position = (Vector2){-50, 300};
    star.velocity = (Vector2){100, 0};
    star.isActive = true;
    
    float oldX = star.position.x;
    UpdateStarItem(&star, 0.1f, 800, 600);
    
    mu_assert_double_eq(oldX + 10.0f, star.position.x); // 100 * 0.1
    mu_assert(star.isActive, "Star should still be active");
}

MU_TEST(test_star_out_of_bounds) {
    StarItem star = InitStarItem();
    star.position = (Vector2){850, 300};
    star.velocity = (Vector2){100, 0};
    star.isActive = true;
    
    UpdateStarItem(&star, 1.0f, 800, 600); // Move far out
    mu_assert(!star.isActive, "Star should deactivate when out of bounds");
}

MU_TEST(test_star_collision) {
    StarItem star = InitStarItem();
    star.position = (Vector2){100, 100};
    star.isActive = true;
    
    // Test collision
    testPlayer.position = (Vector2){100, 100};
    bool collided = CheckStarPlayerCollision(&star, testPlayer.position, testPlayer.size);
    mu_assert(collided, "Should detect collision at same position");
    
    // Test no collision
    testPlayer.position = (Vector2){200, 200};
    collided = CheckStarPlayerCollision(&star, testPlayer.position, testPlayer.size);
    mu_assert(!collided, "Should not detect collision at distance");
}

MU_TEST(test_star_invincibility_trigger) {
    // Manually spawn star
    g_itemManager.starItem.position = (Vector2){100, 100};
    g_itemManager.starItem.isActive = true;
    
    testPlayer.position = (Vector2){100, 100};
    testPlayer.isInvincible = false;
    testPlayer.invincibleTimer = 0.0f;
    
    CheckItemCollisions(&testPlayer);
    
    mu_assert(testPlayer.isInvincible, "Player should become invincible");
    mu_assert_double_eq(STAR_INVINCIBILITY_DURATION, testPlayer.invincibleTimer);
    mu_assert(!g_itemManager.starItem.isActive, "Star should be collected");
}

MU_TEST(test_rainbow_color) {
    // Test color cycling
    Color color1 = GetRainbowColor(0.0f);
    Color color2 = GetRainbowColor(0.5f);
    Color color3 = GetRainbowColor(1.0f);
    
    // Colors should be different
    mu_assert(color1.r != color2.r || color1.g != color2.g || color1.b != color2.b,
              "Colors should change over time");
    
    // Should cycle back
    Color color4 = GetRainbowColor(1.0f);
    Color color5 = GetRainbowColor(0.0f);
    mu_assert(color4.r == color5.r && color4.g == color5.g && color4.b == color5.b,
              "Colors should cycle");
}

MU_TEST(test_star_spawn_interval) {
    InitItemManager();
    
    // Test minimum interval
    g_itemManager.starSpawnTimer = 0.0f;
    mu_assert(g_itemManager.nextStarSpawnInterval >= STAR_SPAWN_MIN_INTERVAL,
              "Spawn interval should be at least minimum");
    mu_assert(g_itemManager.nextStarSpawnInterval <= STAR_SPAWN_MAX_INTERVAL,
              "Spawn interval should not exceed maximum");
}

MU_TEST_SUITE(test_suite_star_item) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_star_init);
    MU_RUN_TEST(test_star_spawn_position);
    MU_RUN_TEST(test_star_movement);
    MU_RUN_TEST(test_star_out_of_bounds);
    MU_RUN_TEST(test_star_collision);
    MU_RUN_TEST(test_star_invincibility_trigger);
    MU_RUN_TEST(test_rainbow_color);
    MU_RUN_TEST(test_star_spawn_interval);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(test_suite_star_item);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

## Validation Gates

```bash
# Build validation
clang -o particles src/*.c src/*/*.c src/*/*/*.c \
    -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib \
    -lraylib -framework OpenGL -framework Cocoa -framework IOKit \
    -std=c11 -Wall -Wextra -Werror

# Syntax/Style check
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem \
    src/entities/items/star_item.c \
    src/entities/managers/item_manager.c

# Unit tests
clang -o test_star tests/unit/test_star_item.c \
    src/entities/items/star_item.c \
    src/entities/managers/item_manager.c \
    src/entities/items/hp_potion.c \
    src/entities/player.c \
    src/core/physics.c \
    src/core/memory_pool.c \
    src/core/event/event_system.c \
    [other required files] \
    -I/opt/homebrew/Cellar/raylib/5.5/include \
    -L/opt/homebrew/Cellar/raylib/5.5/lib \
    -lraylib -framework OpenGL -framework Cocoa -framework IOKit \
    -std=c11 && ./test_star

# Memory leak check (macOS)
leaks --atExit -- ./test_star
```

## Implementation Tasks (In Order)

1. **Create Star Item Entity**
   - [ ] Create `star_item.h` with structure and function declarations
   - [ ] Implement `star_item.c` with all functions
   - [ ] Add rainbow color calculation

2. **Extend Item Manager**
   - [ ] Add StarItem to ItemManager struct
   - [ ] Add star spawn timer and interval logic
   - [ ] Integrate star update and draw calls
   - [ ] Add star collision check

3. **Enhance Player Invincibility**
   - [ ] Add star invincibility flag to player
   - [ ] Update player drawing with rainbow effect
   - [ ] Ensure 20s duration works properly

4. **Build and Test**
   - [ ] Compile with no warnings/errors
   - [ ] Run unit tests
   - [ ] Manual test in game

5. **Audio System (Optional)**
   - [ ] Create basic audio manager structure
   - [ ] Add hooks for star theme music
   - [ ] Integrate with star collection

## Anti-Patterns to Avoid

1. **Don't hardcode screen dimensions** - Always pass as parameters
2. **Don't forget memory pool initialization** - Check before publishing events
3. **Don't mix star and damage invincibility** - Use separate flags
4. **Don't spawn stars too close to screen edge** - Use proper margins
5. **Don't forget to update Makefile** - Add new source files
6. **Don't skip unit tests** - Test all edge cases
7. **Don't ignore deltaTime** - All movement/timers must use it
8. **Don't create global color arrays** - Calculate rainbow dynamically

## Notes

- Audio implementation is marked optional as it requires adding audio files
- Rainbow effect uses HSV to RGB conversion for smooth transitions
- Star trajectory passes through center area but continues beyond
- Consider adding star rotation animation in future enhancement
- Speed boost effect mentioned in requirements can be added to player update

## Confidence Score: 8/10

High confidence due to:
- Clear reference patterns from HP potion
- Established event system and collision detection
- Simple trajectory physics
- Well-defined visual effects

Points deducted for:
- Audio system needs external assets (optional implementation)
- Rainbow color effect is new pattern requiring HSV conversion
- First moving item in the game (trajectory calculation)