name: "Base PRP Template v2 - Context-Rich with Validation Loops"
description: |

## Purpose
Template optimized for AI agents to implement features with sufficient context and self-validation capabilities to achieve working code through iterative refinement.

## Core Principles
1. **Context is King**: Include ALL necessary documentation, examples, and caveats
2. **Validation Loops**: Provide executable tests/lints the AI can run and fix
3. **Information Dense**: Use keywords and patterns from the codebase
4. **Progressive Success**: Start simple, validate, then enhance
5. **Global rules**: Be sure to follow all rules in CLAUDE.md

---

## Goal
[What needs to be built - be specific about the end state and desires]

## Why
- [Business value and user impact]
- [Integration with existing features]
- [Problems this solves and for whom]

## What
[User-visible behavior and technical requirements]

### Success Criteria
- [ ] [Specific measurable outcomes]

## All Needed Context

### Documentation & References (list all context needed to implement the feature)
```yaml
# MUST READ - Include these in your context window
- url: [Official API docs URL]
  why: [Specific sections/methods you'll need]
  
- file: [path/to/example.c]
  why: [Pattern to follow, gotchas to avoid]
  
- doc: [Library documentation URL] 
  section: [Specific section about common pitfalls]
  critical: [Key insight that prevents common errors]

- docfile: [PRPs/ai_docs/file.md]
  why: [docs that the user has pasted in to the project]

```

### Current Codebase tree (run `tree` in the root of the project) to get an overview of the codebase
```bash

```

### Desired Codebase tree with files to be added and responsibility of file
```bash

```


# Implementation Blueprint

This document provides a structured development process for the particle-based game project, including step-by-step guidelines, practical examples, and unit testing templates using MinUnit.

## Table of Contents

1. [Development Process Overview](#development-process-overview)
2. [Phase 1: Planning and Design](#phase-1-planning-and-design)
3. [Phase 2: Core System Implementation](#phase-2-core-system-implementation)
4. [Phase 3: Feature Development](#phase-3-feature-development)
5. [Phase 4: Testing and Quality Assurance](#phase-4-testing-and-quality-assurance)
6. [Phase 5: Performance Optimization](#phase-5-performance-optimization)
7. [Phase 6: Polish and Release](#phase-6-polish-and-release)
8. [Unit Testing Templates](#unit-testing-templates)

## Development Process Overview

### Development Principles
- **Test-Driven Development (TDD)**: Write tests before implementation
- **Incremental Development**: Build features in small, testable chunks
- **Event-Driven Architecture**: Use the event system for component communication
- **Performance-First**: Always consider performance with 100,000 particles
- **Memory Pool Usage**: Prevent allocation overhead during gameplay

### Workflow
1. Plan feature with clear requirements
2. Write unit tests
3. Implement minimum viable feature
4. Test and debug
5. Optimize for performance
6. Document changes

## Phase 1: Planning and Design

### Step 1: Define Feature Requirements
**Goal**: Create clear, testable requirements for each feature

**Process**:
1. Write user stories
2. Define acceptance criteria
3. Identify system dependencies
4. Create technical specifications

**Example: Adding a New Enemy Type**
```markdown
## Feature: Laser Enemy Type

### User Story
As a player, I want to face enemies that shoot laser beams, creating a new challenge that requires positioning strategy.

### Acceptance Criteria
- [ ] Enemy spawns according to stage configuration
- [ ] Laser charges for 2 seconds (visual indicator)
- [ ] Laser fires for 1 second in player's direction
- [ ] Laser damages player on contact (10 damage)
- [ ] Laser can be blocked by particles
- [ ] Enemy has 50 health points

### Technical Requirements
- New enemy type: ENEMY_TYPE_LASER
- Laser rendering system
- Line-based collision detection
- Charge/fire state machine
- Visual effects for charging and firing
```

### Step 2: Design System Architecture
**Goal**: Plan how the feature integrates with existing systems

**Process**:
1. Identify affected systems
2. Design component interactions
3. Plan event flows
4. Create data structures

**Example: Laser Enemy Architecture**
```c
// Data structures
typedef struct {
    Vector2 start;
    Vector2 end;
    float charge_time;
    float fire_time;
    bool is_charging;
    bool is_firing;
} LaserData;

// Event types
typedef enum {
    EVENT_LASER_CHARGE_START,
    EVENT_LASER_FIRE,
    EVENT_LASER_HIT
} LaserEventType;

// System integration points
// - Enemy system: New enemy type and behavior
// - Render system: Laser rendering
// - Physics system: Line collision detection
// - Event system: Laser-specific events
```

## Phase 2: Core System Implementation

### Step 3: Implement Core Components
**Goal**: Build foundation systems that other features depend on

**Process**:
1. Start with data structures
2. Implement basic functionality
3. Add event integration
4. Create unit tests

**Example: Memory Pool Implementation**
```c
// src/core/memory_pool.c
typedef struct {
    void* memory;
    size_t block_size;
    size_t block_count;
    size_t* free_list;
    size_t free_count;
} MemoryPool;

MemoryPool* MemoryPool_Create(size_t block_size, size_t block_count) {
    MemoryPool* pool = malloc(sizeof(MemoryPool));
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->memory = malloc(block_size * block_count);
    pool->free_list = malloc(sizeof(size_t) * block_count);
    pool->free_count = block_count;
    
    // Initialize free list
    for (size_t i = 0; i < block_count; i++) {
        pool->free_list[i] = i;
    }
    
    return pool;
}
```

### Step 4: Write Unit Tests
**Goal**: Ensure core systems work correctly

**Example: Memory Pool Unit Test**
```c
// tests/test_memory_pool.c
#include "../minunit/minunit.h"
#include "../src/core/memory_pool.h"

MU_TEST(test_memory_pool_create) {
    MemoryPool* pool = MemoryPool_Create(sizeof(int), 10);
    
    mu_assert(pool != NULL, "Pool should be created");
    mu_assert_int_eq(10, pool->free_count);
    mu_assert_int_eq(sizeof(int), pool->block_size);
    
    MemoryPool_Destroy(pool);
}

MU_TEST(test_memory_pool_allocate) {
    MemoryPool* pool = MemoryPool_Create(sizeof(int), 5);
    
    int* ptr1 = (int*)MemoryPool_Allocate(pool);
    mu_assert(ptr1 != NULL, "Should allocate successfully");
    mu_assert_int_eq(4, pool->free_count);
    
    *ptr1 = 42;
    mu_assert_int_eq(42, *ptr1);
    
    MemoryPool_Destroy(pool);
}

MU_TEST_SUITE(memory_pool_suite) {
    MU_RUN_TEST(test_memory_pool_create);
    MU_RUN_TEST(test_memory_pool_allocate);
}

int main() {
    MU_RUN_SUITE(memory_pool_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

## Phase 3: Feature Development

### Step 5: Implement Game Features
**Goal**: Build gameplay features using core systems

**Process**:
1. Create feature branch
2. Write feature tests
3. Implement incrementally
4. Test continuously
5. Integrate with game loop

**Example: Enemy Behavior Implementation**
```c
// src/entities/behaviors/laser_enemy_behavior.c
void UpdateLaserEnemy(Enemy* enemy, float deltaTime, Player* player) {
    LaserData* laser = (LaserData*)enemy->custom_data;
    
    switch (enemy->state) {
        case ENEMY_STATE_IDLE:
            // Track player and prepare to charge
            if (Vector2Distance(enemy->position, player->position) < LASER_RANGE) {
                enemy->state = ENEMY_STATE_CHARGING;
                EventSystem_Publish(EVENT_LASER_CHARGE_START, &(LaserChargeEvent){
                    .enemy_id = enemy->id,
                    .position = enemy->position
                });
            }
            break;
            
        case ENEMY_STATE_CHARGING:
            laser->charge_time += deltaTime;
            if (laser->charge_time >= LASER_CHARGE_DURATION) {
                enemy->state = ENEMY_STATE_ATTACKING;
                laser->charge_time = 0;
                laser->is_firing = true;
                
                // Calculate laser direction
                Vector2 direction = Vector2Normalize(
                    Vector2Subtract(player->position, enemy->position)
                );
                laser->end = Vector2Add(enemy->position, 
                    Vector2Scale(direction, LASER_LENGTH));
                
                EventSystem_Publish(EVENT_LASER_FIRE, &(LaserFireEvent){
                    .enemy_id = enemy->id,
                    .start = enemy->position,
                    .end = laser->end
                });
            }
            break;
            
        case ENEMY_STATE_ATTACKING:
            laser->fire_time += deltaTime;
            if (laser->fire_time >= LASER_FIRE_DURATION) {
                enemy->state = ENEMY_STATE_IDLE;
                laser->fire_time = 0;
                laser->is_firing = false;
            }
            break;
    }
}
```

### Step 6: Integration Testing
**Goal**: Ensure features work together correctly

**Example: Stage Integration Test**
```c
// tests/test_stage_integration.c
MU_TEST(test_stage_enemy_spawning) {
    // Initialize game systems
    EventSystem_Init();
    EnemyManager_Init();
    StageManager_Init();
    
    // Start stage 1
    StageManager_StartStage(1);
    
    // Simulate time passage
    for (int i = 0; i < 60; i++) { // 1 second at 60 FPS
        StageManager_Update(1.0f / 60.0f);
        EnemyManager_Update(1.0f / 60.0f);
    }
    
    // Verify enemies spawned
    int enemy_count = EnemyManager_GetEnemyCount();
    mu_assert(enemy_count > 0, "Enemies should spawn in stage 1");
    
    // Cleanup
    StageManager_Cleanup();
    EnemyManager_Cleanup();
    EventSystem_Cleanup();
}
```

## Phase 4: Testing and Quality Assurance

### Step 7: Comprehensive Testing
**Goal**: Ensure game stability and correctness

**Testing Categories**:
1. **Unit Tests**: Individual components
2. **Integration Tests**: System interactions
3. **Performance Tests**: Frame rate and memory usage
4. **Gameplay Tests**: Balance and fun factor

**Example: Performance Test**
```c
// tests/test_performance.c
MU_TEST(test_particle_collision_performance) {
    // Setup
    ParticleManager_Init();
    EnemyManager_Init();
    
    // Create worst-case scenario
    for (int i = 0; i < 100000; i++) {
        ParticleManager_SpawnParticle((Vector2){i % 1000, i / 1000});
    }
    
    for (int i = 0; i < 50; i++) {
        EnemyManager_SpawnEnemy(ENEMY_TYPE_BASIC, 
            (Vector2){rand() % 1000, rand() % 1000});
    }
    
    // Measure collision detection time
    clock_t start = clock();
    
    for (int frame = 0; frame < 60; frame++) {
        Physics_UpdateCollisions(1.0f / 60.0f);
    }
    
    clock_t end = clock();
    double time_spent = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Assert performance requirement (60 frames in less than 1 second)
    mu_assert(time_spent < 1.0, "Collision detection too slow");
    
    // Cleanup
    ParticleManager_Cleanup();
    EnemyManager_Cleanup();
}
```

### Step 8: Memory Leak Detection
**Goal**: Ensure no memory leaks

**Example: Memory Test Script**
```bash
#!/bin/bash
# test_memory_leaks.sh

# Test 1: Build with address sanitizer (detects memory leaks, buffer overflows)
echo "=== Testing with Address Sanitizer ==="
make clean
make CFLAGS="-fsanitize=address -fno-omit-frame-pointer -g" LDFLAGS="-fsanitize=address"
./bin/game --test-mode

# Test 2: Build with leak sanitizer only (lighter weight, just memory leaks)
echo "=== Testing with Leak Sanitizer ==="
make clean
make CFLAGS="-fsanitize=leak -g" LDFLAGS="-fsanitize=leak"
./bin/game --test-mode

# Test 3: Build with memory sanitizer (uninitialized memory usage)
echo "=== Testing with Memory Sanitizer ==="
make clean
make CFLAGS="-fsanitize=memory -fno-omit-frame-pointer -g" LDFLAGS="-fsanitize=memory"
./bin/game --test-mode

# Test 4: Custom memory tracking (built into the game)
echo "=== Testing with Built-in Memory Tracking ==="
make clean
make DEBUG=1 CFLAGS="-DMEMORY_TRACKING=1"
./bin/game --test-mode --memory-report
```

## Phase 5: Performance Optimization (skip)

### Step 9: Profile and Optimize (skip)

### Step 10: Batch Processing (skip)


## Phase 6: Polish and Release

### Step 11: Game Polish
**Goal**: Improve game feel and user experience

**Checklist**:
- [ ] Visual effects for all interactions
- [ ] Sound effects and music
- [ ] UI animations and transitions
- [ ] Particle effects for explosions
- [ ] Screen shake for impacts
- [ ] Tutorial improvements
- [ ] Score entry polish

**Example: Screen Shake Implementation**
```c
// src/effects/screen_shake.c
typedef struct {
    float intensity;
    float duration;
    float elapsed;
    Vector2 offset;
} ScreenShake;

void ScreenShake_Update(ScreenShake* shake, float deltaTime) {
    if (shake->duration > 0) {
        shake->elapsed += deltaTime;
        float progress = shake->elapsed / shake->duration;
        
        if (progress >= 1.0f) {
            shake->duration = 0;
            shake->offset = (Vector2){0, 0};
        } else {
            // Damped shake
            float damping = 1.0f - progress;
            shake->offset.x = (rand() % 200 - 100) / 100.0f * 
                            shake->intensity * damping;
            shake->offset.y = (rand() % 200 - 100) / 100.0f * 
                            shake->intensity * damping;
        }
    }
}
```

### Step 12: Release Preparation
**Goal**: Prepare game for distribution

**Release Checklist**:
1. **Build Configuration**
   - [ ] Release build flags
   - [ ] Platform-specific builds
   - [ ] Asset packaging

2. **Documentation**
   - [ ] README.md update
   - [ ] Controls documentation
   - [ ] System requirements

3. **Testing**
   - [ ] Full playthrough test
   - [ ] Performance on target hardware
   - [ ] Memory usage validation

**Example: Release Build Script**
```bash
# Clean and build the project
make clean
make

# Run the game
make run

# Build and run in one command
make clean && make && make run

# Basic runtime test (runs game for 3 seconds)
./test_memory
```

## Unit Testing Templates

### Basic Test Structure
```c
// tests/test_template.c
#include "../minunit/minunit.h"
#include "../src/module_to_test.h"

// Test fixture - shared setup/teardown
void* test_setup(void) {
    // Allocate and initialize test data
    TestData* data = malloc(sizeof(TestData));
    data->value = 42;
    return data;
}

void test_teardown(void* data) {
    // Clean up test data
    free(data);
}

// Individual test cases
MU_TEST(test_example_functionality) {
    // Arrange
    int expected = 42;
    
    // Act
    int result = FunctionUnderTest(21, 21);
    
    // Assert
    mu_assert_int_eq(expected, result);
}

MU_TEST_WITH_FIXTURE(test_with_setup, test_setup, test_teardown) {
    TestData* data = (TestData*)MU_FIXTURE;
    mu_assert_int_eq(42, data->value);
}

// Test suite
MU_TEST_SUITE(example_suite) {
    MU_RUN_TEST(test_example_functionality);
    MU_RUN_TEST_WITH_FIXTURE(test_with_setup, test_setup, test_teardown);
}

// Main test runner
int main() {
    MU_RUN_SUITE(example_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

### Event System Test Template
```c
// tests/test_event_system.c
#include "../minunit/minunit.h"
#include "../src/core/event/event_system.h"

static int callback_count = 0;
static EventData last_event_data;

void test_callback(EventData data) {
    callback_count++;
    last_event_data = data;
}

MU_TEST(test_event_publish_subscribe) {
    // Setup
    EventSystem_Init();
    callback_count = 0;
    
    // Subscribe
    EventSystem_Subscribe(EVENT_ENEMY_DESTROYED, test_callback);
    
    // Publish event
    EventData data = {.enemy_id = 123};
    EventSystem_Publish(EVENT_ENEMY_DESTROYED, data);
    
    // Verify
    mu_assert_int_eq(1, callback_count);
    mu_assert_int_eq(123, last_event_data.enemy_id);
    
    // Cleanup
    EventSystem_Cleanup();
}
```

### Physics Test Template
```c
// tests/test_physics.c
#include "../minunit/minunit.h"
#include "../src/core/physics.h"

MU_TEST(test_collision_detection) {
    // Setup entities
    Vector2 pos1 = {100, 100};
    Vector2 pos2 = {110, 100};
    float radius1 = 10;
    float radius2 = 10;
    
    // Test collision
    bool collides = Physics_CircleCollision(pos1, radius1, pos2, radius2);
    mu_assert(collides, "Circles should collide");
    
    // Test non-collision
    pos2.x = 200;
    collides = Physics_CircleCollision(pos1, radius1, pos2, radius2);
    mu_assert(!collides, "Circles should not collide");
}

MU_TEST(test_attraction_force) {
    Vector2 particle_pos = {0, 0};
    Vector2 player_pos = {100, 0};
    float attraction_strength = 100.0f;
    
    Vector2 force = Physics_CalculateAttraction(
        particle_pos, player_pos, attraction_strength
    );
    
    // Force should point towards player
    mu_assert(force.x > 0, "Force X should be positive");
    mu_assert_double_eq(0.0, force.y);
}
```

### Performance Test Template
```c
// tests/test_performance.c
#include "../minunit/minunit.h"
#include <time.h>

MU_TEST(test_performance_benchmark) {
    const int ITERATIONS = 1000000;
    
    clock_t start = clock();
    
    // Perform operation many times
    for (int i = 0; i < ITERATIONS; i++) {
        PerformanceOperation();
    }
    
    clock_t end = clock();
    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Assert performance requirement
    double max_time = 1.0; // 1 second max
    mu_assert(elapsed < max_time, "Operation too slow");
    
    // Log performance for tracking
    printf("Performance: %d operations in %.3f seconds\n", 
           ITERATIONS, elapsed);
}
```

### Integration Test Template
```c
// tests/test_integration.c
#include "../minunit/minunit.h"

MU_TEST(test_game_loop_integration) {
    // Initialize all systems
    Game_Init();
    
    // Simulate game frames
    const int FRAMES = 300; // 5 seconds at 60 FPS
    
    for (int i = 0; i < FRAMES; i++) {
        Game_Update(1.0f / 60.0f);
        
        // Verify invariants
        mu_assert(ParticleManager_GetCount() <= MAX_PARTICLES, 
                 "Particle count exceeded");
        mu_assert(Game_GetFPS() >= 30, "FPS too low");
    }
    
    // Verify end state
    GameState* state = Game_GetState();
    mu_assert(state->score >= 0, "Invalid score");
    mu_assert(state->player.health > 0 || state->game_over, 
             "Invalid game state");
    
    // Cleanup
    Game_Cleanup();
}
```

## Test Organization

### Directory Structure for Tests
```
particles/
├── tests/
│   ├── unit/           # Unit tests for individual components
│   │   ├── test_memory_pool.c
│   │   ├── test_event_system.c
│   │   ├── test_physics.c
│   │   └── test_enemy_behaviors.c
│   ├── integration/    # Integration tests
│   │   ├── test_game_systems.c
│   │   ├── test_stage_progression.c
│   │   └── test_collision_system.c
│   ├── performance/    # Performance tests
│   │   ├── test_particle_performance.c
│   │   └── test_render_performance.c
│   └── test_runner.c   # Main test runner
```

### Test Runner Script
```c
// tests/test_runner.c
#include "../minunit/minunit.h"

// Include all test suites
extern void memory_pool_suite();
extern void event_system_suite();
extern void physics_suite();
extern void integration_suite();

int main() {
    // Run all test suites
    MU_RUN_SUITE(memory_pool_suite);
    MU_RUN_SUITE(event_system_suite);
    MU_RUN_SUITE(physics_suite);
    MU_RUN_SUITE(integration_suite);
    
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

### Makefile Test Target
```makefile
# Add to Makefile
test: $(TEST_OBJS)
    $(CC) $(TEST_OBJS) -o bin/test_runner $(LDFLAGS)
    ./bin/test_runner

test-verbose: $(TEST_OBJS)
    $(CC) $(TEST_OBJS) -o bin/test_runner $(LDFLAGS)
    ./bin/test_runner -v

test-performance: $(PERF_TEST_OBJS)
    $(CC) $(PERF_TEST_OBJS) -o bin/test_performance $(LDFLAGS)
    ./bin/test_performance

.PHONY: test test-verbose test-performance
```

## Continuous Integration (skip)

### GitHub Actions Workflow (skip)


## Summary

This Implementation Blueprint provides a structured approach to developing the particle-based game:

1. **Planning**: Define clear requirements and architecture
2. **Core Systems**: Build foundation with tests
3. **Features**: Implement incrementally with TDD
4. **Testing**: Comprehensive quality assurance
5. **Optimization**: Profile and improve performance
6. **Polish**: Enhance user experience
7. **Release**: Prepare for distribution

Each phase includes:
- Clear goals and processes
- Practical code examples
- Unit test templates using MinUnit
- Integration strategies
- Performance considerations

Following this blueprint ensures a well-structured, testable, and performant game implementation.
---

## Anti-Patterns to Avoid
- ❌ Don't create new patterns when existing ones work
- ❌ Don't skip validation because "it should work"  
- ❌ Don't ignore failing tests - fix them
- ❌ Don't hardcode values that should be config
