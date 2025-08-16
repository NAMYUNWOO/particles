name: "HP Potion Item Implementation PRP"
description: |

## Purpose
Implement an HP potion pickup item that spawns randomly on the map, restores player health when collected, and includes visual feedback for its lifecycle.

## Core Principles
1. **Context is King**: Include ALL necessary documentation, examples, and caveats
2. **Validation Loops**: Provide executable tests/lints the AI can run and fix
3. **Information Dense**: Use keywords and patterns from the codebase
4. **Progressive Success**: Start simple, validate, then enhance
5. **Global rules**: Be sure to follow all rules in CLAUDE.md

---

## Goal
Implement a collectible HP potion item that:
- Spawns at random visible locations every 25 seconds
- Exists for 10 seconds (blinking in last 3 seconds)
- Restores player health on contact (if not full)
- Uses event-driven architecture for interactions
- Follows memory pool patterns for performance

## Why
- **Player Recovery**: Provides strategic health recovery during intense battles
- **Resource Management**: Adds risk/reward decisions (move to collect vs. stay safe)
- **Game Balance**: Helps players survive longer in difficult stages
- **Visual Polish**: Adds more interactive elements to the game world

## What
### Functional Requirements
- HP potion spawns every 25 seconds at random visible location
- Only one potion can exist at a time
- Potion has HPPOTION_BASE_SIZE = 10.0f radius
- Potion is stationary (not moving)
- Potion exists for 10 seconds total
- Last 3 seconds: potion blinks to warn of disappearance
- On player contact: restore health if not full
- Uses event system for spawn/collect/expire events

### Success Criteria
- [ ] Potion spawns correctly with timer system
- [ ] Visual rendering with proper size and color
- [ ] Collision detection with player works
- [ ] Health restoration logic functions correctly
- [ ] Blinking animation in last 3 seconds
- [ ] Memory pool usage for event data
- [ ] Unit tests pass for all components

## All Needed Context

### Documentation & References
```yaml
# MUST READ - Include these in your context window
- url: https://www.raylib.com/cheatsheet/cheatsheet.html
  why: CheckCollisionCircles, DrawCircle, GetTime functions
  
- file: src/entities/enemy.c
  why: Pattern for entity spawning, timer management, collision handling
  
- file: src/entities/managers/enemy_manager.c
  why: Example of spawn timing, entity management patterns
  
- file: src/core/event/event_system.h
  why: Event system patterns, how to define and publish events
  
- file: src/core/physics.c
  why: Collision detection patterns, memory pool usage for events
  
- file: src/entities/player.h
  why: Player structure, health system, position tracking
  
- file: src/core/memory_pool.h
  why: Memory pool allocation patterns
  
- file: RAYLIB_CONVENTIONS.md
  why: Code style and naming conventions
  
- file: examples/minunit/minunit_example.c
  why: Unit test structure and patterns
```

### Current Codebase tree
```bash
src/
├── core/
│   ├── event/
│   │   ├── event_system.c/h
│   │   └── event_types.h
│   ├── game.c/h
│   ├── physics.c/h
│   └── memory_pool.c/h
├── entities/
│   ├── enemy.c/h
│   ├── player.c/h
│   ├── particle.c/h
│   └── managers/
│       ├── enemy_manager.c/h
│       ├── particle_manager.c/h
│       └── stage_manager.c/h
└── main.c
```

### Desired Codebase tree with files to be added
```bash
src/
├── core/
│   ├── event/
│   │   ├── event_system.c/h
│   │   └── event_types.h (MODIFIED - add potion events)
│   ├── game.c/h (MODIFIED - add potion to game state)
│   ├── physics.c/h (MODIFIED - add potion collision check)
│   └── memory_pool.c/h
├── entities/
│   ├── items/
│   │   ├── hp_potion.c (NEW - potion logic implementation)
│   │   └── hp_potion.h (NEW - potion structure and interface)
│   ├── managers/
│   │   ├── item_manager.c (NEW - manages all items including potions)
│   │   └── item_manager.h (NEW - item manager interface)
│   └── [existing files...]
└── main.c (MODIFIED - initialize item system)

tests/
└── unit/
    ├── test_hp_potion.c (NEW - unit tests for potion)
    └── test_item_manager.c (NEW - unit tests for item manager)
```

# Implementation Blueprint

## Step-by-Step Implementation Plan

### Phase 1: Core Data Structures and Events

#### 1.1 Define HP Potion Structure (hp_potion.h)
```c
#ifndef HP_POTION_H
#define HP_POTION_H

#include "raylib.h"

#define HPPOTION_BASE_SIZE 10.0f
#define POTION_LIFETIME 10.0f
#define POTION_BLINK_TIME 3.0f
#define POTION_SPAWN_INTERVAL 25.0f

typedef struct {
    Vector2 position;
    float radius;
    float lifetime;           // Time remaining before disappearing
    bool isActive;           // Whether potion exists
    bool isBlinking;         // Whether in blinking phase
    float blinkTimer;        // For blink animation
} HPPotion;

// Function declarations
HPPotion InitHPPotion(void);
void SpawnHPPotion(HPPotion* potion, int screenWidth, int screenHeight);
void UpdateHPPotion(HPPotion* potion, float deltaTime);
void DrawHPPotion(HPPotion potion);
bool CheckPotionPlayerCollision(HPPotion potion, Vector2 playerPos, float playerRadius);

#endif // HP_POTION_H
```

#### 1.2 Add Potion Events to event_types.h
```c
// Add to event_types.h after existing event types

// Item-related events
EVENT_ITEM_SPAWNED,          // When any item spawns
EVENT_ITEM_COLLECTED,        // When player collects item
EVENT_ITEM_EXPIRED,          // When item disappears
EVENT_HP_RESTORED,           // When player health is restored

// Add corresponding event data structures:

// Item event data
typedef struct {
    int itemType;    // 0: HP Potion, future: other items
    Vector2 position;
    void* itemPtr;
} ItemEventData;

// Health restoration event data
typedef struct {
    int oldHealth;
    int newHealth;
    int amountRestored;
} HealthRestoredEventData;
```

### Phase 2: Implementation

#### 2.1 HP Potion Implementation (hp_potion.c)
```c
#include "hp_potion.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include <stdlib.h>

HPPotion InitHPPotion(void) {
    HPPotion potion = {
        .position = (Vector2){0, 0},
        .radius = HPPOTION_BASE_SIZE,
        .lifetime = 0.0f,
        .isActive = false,
        .isBlinking = false,
        .blinkTimer = 0.0f
    };
    return potion;
}

void SpawnHPPotion(HPPotion* potion, int screenWidth, int screenHeight) {
    if (potion->isActive) return; // Already active
    
    // Random position within screen bounds (with margin)
    float margin = HPPOTION_BASE_SIZE * 2;
    potion->position.x = margin + (rand() % (int)(screenWidth - 2 * margin));
    potion->position.y = margin + (rand() % (int)(screenHeight - 2 * margin));
    
    potion->lifetime = POTION_LIFETIME;
    potion->isActive = true;
    potion->isBlinking = false;
    potion->blinkTimer = 0.0f;
    
    // Publish spawn event
    extern MemoryPool g_itemEventPool; // Defined in physics.c
    ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
    if (data) {
        data->itemType = 0; // HP Potion
        data->position = potion->position;
        data->itemPtr = potion;
        PublishEvent(EVENT_ITEM_SPAWNED, data);
    }
}

void UpdateHPPotion(HPPotion* potion, float deltaTime) {
    if (!potion->isActive) return;
    
    potion->lifetime -= deltaTime;
    
    // Check if should start blinking
    if (potion->lifetime <= POTION_BLINK_TIME && !potion->isBlinking) {
        potion->isBlinking = true;
    }
    
    // Update blink timer
    if (potion->isBlinking) {
        potion->blinkTimer += deltaTime;
    }
    
    // Check if expired
    if (potion->lifetime <= 0) {
        potion->isActive = false;
        
        // Publish expire event
        extern MemoryPool g_itemEventPool;
        ItemEventData* data = MemoryPool_Alloc(&g_itemEventPool);
        if (data) {
            data->itemType = 0;
            data->position = potion->position;
            data->itemPtr = potion;
            PublishEvent(EVENT_ITEM_EXPIRED, data);
        }
    }
}

void DrawHPPotion(HPPotion potion) {
    if (!potion.isActive) return;
    
    // Determine visibility for blinking
    bool visible = true;
    if (potion.isBlinking) {
        // Blink every 0.3 seconds
        visible = ((int)(potion.blinkTimer / 0.3f) % 2) == 0;
    }
    
    if (visible) {
        // Draw potion as a red circle with white cross
        DrawCircle(potion.position.x, potion.position.y, potion.radius, RED);
        
        // Draw white cross
        float crossSize = potion.radius * 0.6f;
        DrawRectangle(
            potion.position.x - crossSize, 
            potion.position.y - 2, 
            crossSize * 2, 
            4, 
            WHITE
        );
        DrawRectangle(
            potion.position.x - 2, 
            potion.position.y - crossSize, 
            4, 
            crossSize * 2, 
            WHITE
        );
    }
}

bool CheckPotionPlayerCollision(HPPotion potion, Vector2 playerPos, float playerRadius) {
    if (!potion.isActive) return false;
    
    return CheckCollisionCircles(
        potion.position, potion.radius,
        playerPos, playerRadius
    );
}
```

#### 2.2 Item Manager Implementation (item_manager.h/c)
```c
// item_manager.h
#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "hp_potion.h"
#include "../player.h"

typedef struct {
    HPPotion hpPotion;
    float potionSpawnTimer;
    bool initialized;
} ItemManager;

// Global item manager instance
extern ItemManager g_itemManager;

void InitItemManager(void);
void CleanupItemManager(void);
void UpdateItemManager(float deltaTime, int screenWidth, int screenHeight);
void DrawItems(void);
void CheckItemCollisions(Player* player);

#endif // ITEM_MANAGER_H
```

```c
// item_manager.c
#include "item_manager.h"
#include "../../core/event/event_system.h"
#include "../../core/event/event_types.h"
#include "../../core/memory_pool.h"

ItemManager g_itemManager = {0};

// Memory pools for item events
MemoryPool g_itemEventPool;
MemoryPool g_healthEventPool;

void InitItemManager(void) {
    g_itemManager.hpPotion = InitHPPotion();
    g_itemManager.potionSpawnTimer = 0.0f;
    g_itemManager.initialized = true;
    
    // Initialize memory pools
    MemoryPool_Init(&g_itemEventPool, sizeof(ItemEventData), 32);
    MemoryPool_Init(&g_healthEventPool, sizeof(HealthRestoredEventData), 16);
}

void CleanupItemManager(void) {
    g_itemManager.initialized = false;
    MemoryPool_Destroy(&g_itemEventPool);
    MemoryPool_Destroy(&g_healthEventPool);
}

void UpdateItemManager(float deltaTime, int screenWidth, int screenHeight) {
    if (!g_itemManager.initialized) return;
    
    // Update spawn timer
    g_itemManager.potionSpawnTimer += deltaTime;
    
    // Check if should spawn potion
    if (g_itemManager.potionSpawnTimer >= POTION_SPAWN_INTERVAL && 
        !g_itemManager.hpPotion.isActive) {
        SpawnHPPotion(&g_itemManager.hpPotion, screenWidth, screenHeight);
        g_itemManager.potionSpawnTimer = 0.0f;
    }
    
    // Update existing potion
    UpdateHPPotion(&g_itemManager.hpPotion, deltaTime);
}

void DrawItems(void) {
    if (!g_itemManager.initialized) return;
    DrawHPPotion(g_itemManager.hpPotion);
}

void CheckItemCollisions(Player* player) {
    if (!g_itemManager.initialized) return;
    
    // Check HP potion collision
    if (CheckPotionPlayerCollision(
            g_itemManager.hpPotion, 
            player->position, 
            player->size)) {
        
        // Only collect if player needs health
        if (player->health < 3) {
            int oldHealth = player->health;
            player->health = 3; // Full restore
            
            // Deactivate potion
            g_itemManager.hpPotion.isActive = false;
            
            // Publish collected event
            ItemEventData* itemData = MemoryPool_Alloc(&g_itemEventPool);
            if (itemData) {
                itemData->itemType = 0;
                itemData->position = g_itemManager.hpPotion.position;
                itemData->itemPtr = &g_itemManager.hpPotion;
                PublishEvent(EVENT_ITEM_COLLECTED, itemData);
            }
            
            // Publish health restored event
            HealthRestoredEventData* healthData = MemoryPool_Alloc(&g_healthEventPool);
            if (healthData) {
                healthData->oldHealth = oldHealth;
                healthData->newHealth = player->health;
                healthData->amountRestored = player->health - oldHealth;
                PublishEvent(EVENT_HP_RESTORED, healthData);
            }
        }
    }
}
```

### Phase 3: Integration

#### 3.1 Modify game.h to include item manager
```c
// Add to game.h
#include "../entities/managers/item_manager.h"

// In Game struct, add:
// Item management
ItemManager* itemManager;  // Pointer to global item manager
```

#### 3.2 Modify game.c to initialize and update items
```c
// In InitGame function:
InitItemManager();
game.itemManager = &g_itemManager;

// In UpdateGame function (in GAME_STATE_PLAYING case):
UpdateItemManager(game->deltaTime, game->screenWidth, game->screenHeight);
CheckItemCollisions(&game->player);

// In DrawGame function (in GAME_STATE_PLAYING case):
DrawItems();

// In CleanupGame function:
CleanupItemManager();
```

#### 3.3 Modify physics.c to declare external memory pools
```c
// Add these declarations near other pool declarations:
extern MemoryPool g_itemEventPool;
extern MemoryPool g_healthEventPool;
```

### Phase 4: Unit Tests

#### 4.1 HP Potion Unit Tests (test_hp_potion.c)
```c
#include "../../src/minunit/minunit.h"
#include "../../src/entities/items/hp_potion.h"

static HPPotion testPotion;

void test_setup(void) {
    testPotion = InitHPPotion();
}

void test_teardown(void) {
    // Nothing to clean up
}

MU_TEST(test_hp_potion_init) {
    mu_assert(!testPotion.isActive, "Potion should start inactive");
    mu_assert_double_eq(HPPOTION_BASE_SIZE, testPotion.radius);
    mu_assert_double_eq(0.0f, testPotion.lifetime);
}

MU_TEST(test_hp_potion_spawn) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    mu_assert(testPotion.isActive, "Potion should be active after spawn");
    mu_assert_double_eq(POTION_LIFETIME, testPotion.lifetime);
    mu_assert(testPotion.position.x >= HPPOTION_BASE_SIZE * 2, "X position in bounds");
    mu_assert(testPotion.position.x <= 800 - HPPOTION_BASE_SIZE * 2, "X position in bounds");
    mu_assert(testPotion.position.y >= HPPOTION_BASE_SIZE * 2, "Y position in bounds");
    mu_assert(testPotion.position.y <= 600 - HPPOTION_BASE_SIZE * 2, "Y position in bounds");
}

MU_TEST(test_hp_potion_update_lifetime) {
    SpawnHPPotion(&testPotion, 800, 600);
    float initialLifetime = testPotion.lifetime;
    
    UpdateHPPotion(&testPotion, 1.0f);
    
    mu_assert_double_eq(initialLifetime - 1.0f, testPotion.lifetime);
    mu_assert(testPotion.isActive, "Potion still active");
}

MU_TEST(test_hp_potion_blink_phase) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    // Fast forward to blink phase
    UpdateHPPotion(&testPotion, POTION_LIFETIME - POTION_BLINK_TIME + 0.1f);
    
    mu_assert(testPotion.isBlinking, "Potion should be blinking");
    mu_assert(testPotion.isActive, "Potion still active while blinking");
}

MU_TEST(test_hp_potion_expire) {
    SpawnHPPotion(&testPotion, 800, 600);
    
    // Fast forward past lifetime
    UpdateHPPotion(&testPotion, POTION_LIFETIME + 0.1f);
    
    mu_assert(!testPotion.isActive, "Potion should be inactive after expiring");
}

MU_TEST(test_hp_potion_collision) {
    SpawnHPPotion(&testPotion, 400, 300);
    
    // Test collision with player at same position
    Vector2 playerPos = {400, 300};
    float playerRadius = 10.0f;
    
    bool collides = CheckPotionPlayerCollision(testPotion, playerPos, playerRadius);
    mu_assert(collides, "Should detect collision at same position");
    
    // Test no collision when far away
    playerPos.x = 500;
    playerPos.y = 400;
    collides = CheckPotionPlayerCollision(testPotion, playerPos, playerRadius);
    mu_assert(!collides, "Should not detect collision when far away");
}

MU_TEST_SUITE(hp_potion_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_hp_potion_init);
    MU_RUN_TEST(test_hp_potion_spawn);
    MU_RUN_TEST(test_hp_potion_update_lifetime);
    MU_RUN_TEST(test_hp_potion_blink_phase);
    MU_RUN_TEST(test_hp_potion_expire);
    MU_RUN_TEST(test_hp_potion_collision);
}

int main() {
    MU_RUN_SUITE(hp_potion_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

### Phase 5: Validation Gates

#### 5.1 Compilation Check
```bash
# Ensure code compiles without warnings
make clean
make CFLAGS="-Wall -Wextra -Werror"
```

#### 5.2 Unit Test Execution
```bash
# Compile and run unit tests
gcc -o test_hp_potion tests/unit/test_hp_potion.c src/entities/items/hp_potion.c -lraylib -lm
./test_hp_potion

gcc -o test_item_manager tests/unit/test_item_manager.c src/entities/managers/item_manager.c src/entities/items/hp_potion.c -lraylib -lm
./test_item_manager
```

#### 5.3 Memory Leak Check
```bash
# Build with address sanitizer
make clean
make CFLAGS="-fsanitize=address -fno-omit-frame-pointer -g" LDFLAGS="-fsanitize=address"
./particles
```

#### 5.4 Integration Test
```bash
# Run game and verify:
# 1. Potion spawns after 25 seconds
# 2. Potion blinks in last 3 seconds
# 3. Player health restores on collection
# 4. Only one potion exists at a time
make run
```

### Phase 6: Implementation Tasks (in order)

1. **Create hp_potion.h/c files**
   - Define HPPotion structure
   - Implement spawn, update, draw, collision functions
   
2. **Update event_types.h**
   - Add new event types for items
   - Add event data structures
   
3. **Create item_manager.h/c files**
   - Implement spawn timer logic
   - Handle item updates and collisions
   - Initialize memory pools
   
4. **Integrate with game system**
   - Modify game.h to include item manager
   - Update game.c initialization and update loops
   - Add drawing calls
   
5. **Create unit tests**
   - Test potion lifecycle
   - Test collision detection
   - Test item manager timing
   
6. **Run validation gates**
   - Compile with strict warnings
   - Run unit tests
   - Check for memory leaks
   - Manual integration testing

## Anti-Patterns to Avoid
- ❌ Don't use malloc/free for event data - use memory pools
- ❌ Don't spawn items outside screen bounds
- ❌ Don't allow multiple potions to exist simultaneously
- ❌ Don't restore health if player is already at full health
- ❌ Don't forget to handle the blinking animation timing
- ❌ Don't skip unit tests - they ensure correctness

## Common Pitfalls & Solutions
1. **Random position generation**: Ensure margins prevent spawn at screen edges
2. **Blink timing**: Use modulo operation for consistent blink intervals
3. **Memory leaks**: Always use memory pools for event data
4. **Event handling**: Free event data after processing in handlers
5. **Timer precision**: Use float for timers, not int

## Confidence Score: 9/10
This PRP provides comprehensive context including:
- Complete implementation with working code examples
- Integration points clearly defined
- Unit test coverage
- Memory management patterns from codebase
- Visual rendering specifications
- Event system integration
- Validation gates for quality assurance

The implementation follows existing patterns in the codebase and should succeed in one pass.