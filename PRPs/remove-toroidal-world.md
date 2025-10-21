name: "Remove Toroidal World - Boundary Enforcement v1.0"
description: |
  Remove toroidal world wrapping behavior where particles wrap around screen edges. 
  Implement consistent boundary enforcement where no objects can go over screen boundaries.

## Goal
Transform the game from a toroidal topology (wrap-around edges) to a bounded rectangle where all entities (particles, enemies, player) respect screen boundaries and cannot go beyond them.

## Why
- **Gameplay clarity**: Players expect objects to stop at visible boundaries
- **Visual consistency**: All entities should behave similarly at edges
- **Strategic depth**: Corners and edges become tactical positions
- **Performance**: Simpler distance calculations without toroidal math

## What
Convert particle movement system from wrap-around to boundary-respecting behavior, matching the existing player and enemy boundary handling.

### Success Criteria
- [ ] Particles bounce off screen edges instead of wrapping
- [ ] Toroidal distance calculations removed 
- [ ] Normal attraction works correctly near boundaries
- [ ] All 100,000 particles stay within screen bounds
- [ ] Performance maintained or improved
- [ ] Unit tests pass for new boundary behavior

## All Needed Context

### Documentation & References
```yaml
- file: src/entities/particle.c
  why: Contains MoveParticle() with wrapping logic (lines 126-140)
  
- file: src/entities/particle.h
  why: Interface with toroidal functions to remove/modify
  
- file: src/entities/player.c
  why: Reference implementation for boundary clamping (lines 95-98)
  
- file: src/entities/enemy.c
  why: Reference for boundary bouncing (lines 478-492)
  
- file: src/entities/managers/particle_manager.c
  why: Uses AttractParticleToroidal, needs update to normal attraction
  
- file: src/core/game.c
  why: Contains GetToroidalDirection usage that needs removal

- file: tests/unit/test_hp_potion.c
  why: Example test structure using minunit
  
- file: CLAUDE.md
  why: Project conventions and guidelines
  
- file: PLANNING.md
  why: Architecture overview and performance considerations
```

### Current Implementation Analysis

#### Wrapping Behavior (TO REMOVE)
```c
// src/entities/particle.c - Lines 126-140
void MoveParticle(Particle* particle, int screenWidth, int screenHeight) {
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    
    // REMOVE THIS: Toroidal wrapping
    if (particle->position.x < 0)
        particle->position.x += screenWidth;
    if (particle->position.x >= screenWidth)
        particle->position.x -= screenWidth;
    if (particle->position.y < 0)
        particle->position.y += screenHeight;
    if (particle->position.y >= screenHeight)
        particle->position.y -= screenHeight;
}
```

#### Toroidal Functions (TO REMOVE/REPLACE)
```c
// Functions to remove from particle.h/particle.c:
- Vector2 GetToroidalDirection(...)
- void AttractParticleToroidal(...)

// Usage to replace in particle_manager.c:
AttractParticleToroidal(&game->particles[i], playerCenter, force, 
                       game->screenWidth, game->screenHeight);
// Replace with:
AttractParticle(&game->particles[i], playerCenter, force);
```

#### Reference: Enemy Bouncing Pattern
```c
// src/entities/enemy.c - Bounce implementation
if (enemy->position.x < -margin) {
    enemy->position.x = -margin;
    enemy->velocity.x *= -1;  // Reverse direction
}
if (enemy->position.x > screenWidth + margin) {
    enemy->position.x = screenWidth + margin;
    enemy->velocity.x *= -1;
}
```

#### Reference: Player Clamping Pattern  
```c
// src/entities/player.c - Clamping implementation
if (player->position.x < 0) player->position.x = 0;
if (player->position.x > screenWidth - player->size) 
    player->position.x = screenWidth - player->size;
```

## Implementation Blueprint

### Phase 1: Update Particle Movement

#### Step 1: Modify MoveParticle function
```c
// src/entities/particle.c
void MoveParticle(Particle* particle, int screenWidth, int screenHeight) {
    // Update position
    particle->position.x += particle->velocity.x;
    particle->position.y += particle->velocity.y;
    
    // Bounce off boundaries (like enemies)
    // X boundaries
    if (particle->position.x <= 0) {
        particle->position.x = 0;
        particle->velocity.x = fabs(particle->velocity.x);  // Ensure positive
    }
    if (particle->position.x >= screenWidth - 1) {
        particle->position.x = screenWidth - 1;
        particle->velocity.x = -fabs(particle->velocity.x);  // Ensure negative
    }
    
    // Y boundaries  
    if (particle->position.y <= 0) {
        particle->position.y = 0;
        particle->velocity.y = fabs(particle->velocity.y);
    }
    if (particle->position.y >= screenHeight - 1) {
        particle->position.y = screenHeight - 1;
        particle->velocity.y = -fabs(particle->velocity.y);
    }
}
```

### Phase 2: Update Attraction System

#### Step 2: Simplify AttractParticle function
```c
// Already exists in particle.c, just ensure it's correct:
void AttractParticle(Particle* particle, Vector2 posToAttract, float multiplier) {
    float dist = fmaxf(GetParticleDistance(*particle, posToAttract), 0.5f);
    Vector2 normal = GetParticleNormal(*particle, posToAttract);
    
    particle->velocity.x -= normal.x/dist * multiplier;
    particle->velocity.y -= normal.y/dist * multiplier;
}
```

#### Step 3: Update particle_manager.c
```c
// src/entities/managers/particle_manager.c
// Find and replace all AttractParticleToroidal calls
// FROM:
AttractParticleToroidal(&game->particles[i], playerCenter, 
                       BOOSTED_ATTRACTION_FORCE, 
                       game->screenWidth, game->screenHeight);
// TO:
AttractParticle(&game->particles[i], playerCenter, BOOSTED_ATTRACTION_FORCE);
```

### Phase 3: Remove Toroidal Code

#### Step 4: Clean up particle.h
```c
// Remove from particle.h:
// - void AttractParticleToroidal(...);
// - Vector2 GetToroidalDirection(...);

// Final particle.h physics functions:
void AttractParticle(Particle* particle, Vector2 target, float force);
void ApplyFriction(Particle* particle, float frictionCoeff);
void MoveParticle(Particle* particle, int screenWidth, int screenHeight);
float GetParticleDistance(Particle particle, Vector2 otherPos);
Vector2 GetParticleNormal(Particle particle, Vector2 otherPos);
```

#### Step 5: Remove toroidal implementations from particle.c
- Delete `GetToroidalDirection` function (lines ~47-79)
- Delete `AttractParticleToroidal` function (lines ~106-117)

### Phase 4: Update Game Logic

#### Step 6: Update game.c
Search and replace all uses of `GetToroidalDirection` with direct vector calculations:
```c
// FROM:
Vector2 repulseDir = GetToroidalDirection(game->enemies[i].position,
                                          game->particles[p].position,
                                          game->screenWidth, game->screenHeight, &dist);
// TO:
float dx = game->particles[p].position.x - game->enemies[i].position.x;
float dy = game->particles[p].position.y - game->enemies[i].position.y;
float dist = sqrtf(dx*dx + dy*dy);
Vector2 repulseDir = {0, 0};
if (dist > 0.0f) {
    repulseDir.x = dx / dist;
    repulseDir.y = dy / dist;
}
```

### Phase 5: Testing

#### Step 7: Create unit tests
```c
// tests/unit/test_particle_boundaries.c
#include "../../src/minunit/minunit.h"
#include "../../src/entities/particle.h"

void test_setup(void) {
    // Initialize test environment
}

void test_teardown(void) {
    // Cleanup
}

MU_TEST(test_particle_bounce_left_boundary) {
    Particle p = {
        .position = {0, 400},
        .velocity = {-5.0f, 0},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.x);
    mu_assert(p.velocity.x > 0, "Velocity should reverse to positive");
}

MU_TEST(test_particle_bounce_right_boundary) {
    Particle p = {
        .position = {799, 400},
        .velocity = {5.0f, 0},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(799.0, p.position.x);
    mu_assert(p.velocity.x < 0, "Velocity should reverse to negative");
}

MU_TEST(test_particle_bounce_top_boundary) {
    Particle p = {
        .position = {400, 0},
        .velocity = {0, -5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.y);
    mu_assert(p.velocity.y > 0, "Velocity should reverse to positive");
}

MU_TEST(test_particle_bounce_bottom_boundary) {
    Particle p = {
        .position = {400, 599},
        .velocity = {0, 5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(599.0, p.position.y);
    mu_assert(p.velocity.y < 0, "Velocity should reverse to negative");
}

MU_TEST(test_particle_bounce_corner) {
    Particle p = {
        .position = {0, 0},
        .velocity = {-5.0f, -5.0f},
        .color = BLACK
    };
    
    MoveParticle(&p, 800, 600);
    
    mu_assert_double_eq(0.0, p.position.x);
    mu_assert_double_eq(0.0, p.position.y);
    mu_assert(p.velocity.x > 0, "X velocity should be positive");
    mu_assert(p.velocity.y > 0, "Y velocity should be positive");
}

MU_TEST(test_particle_stays_in_bounds) {
    // Test 1000 random movements
    for (int i = 0; i < 1000; i++) {
        Particle p = InitParticle(800, 600);
        
        // Move particle 100 times
        for (int j = 0; j < 100; j++) {
            MoveParticle(&p, 800, 600);
            
            // Assert particle stays in bounds
            mu_assert(p.position.x >= 0 && p.position.x < 800, 
                     "X position out of bounds");
            mu_assert(p.position.y >= 0 && p.position.y < 600, 
                     "Y position out of bounds");
        }
    }
}

MU_TEST_SUITE(particle_boundary_suite) {
    MU_SUITE_CONFIGURE(&test_setup, &test_teardown);
    
    MU_RUN_TEST(test_particle_bounce_left_boundary);
    MU_RUN_TEST(test_particle_bounce_right_boundary);
    MU_RUN_TEST(test_particle_bounce_top_boundary);
    MU_RUN_TEST(test_particle_bounce_bottom_boundary);
    MU_RUN_TEST(test_particle_bounce_corner);
    MU_RUN_TEST(test_particle_stays_in_bounds);
}

int main(int argc, char *argv[]) {
    MU_RUN_SUITE(particle_boundary_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
```

## Implementation Tasks (In Order)

1. **Update MoveParticle function** in `src/entities/particle.c`
   - Replace wrapping logic with bouncing logic
   - Ensure particles stay at exact boundary (0 or screenWidth-1)

2. **Update particle_manager.c**
   - Replace all `AttractParticleToroidal` calls with `AttractParticle`
   - Remove screenWidth/screenHeight parameters from attraction calls

3. **Clean up particle.h interface**
   - Remove toroidal function declarations
   - Update comments to reflect new behavior

4. **Remove toroidal functions from particle.c**
   - Delete `GetToroidalDirection` function
   - Delete `AttractParticleToroidal` function

5. **Update game.c**
   - Replace `GetToroidalDirection` calls with direct calculations
   - Ensure repulsion/blackhole effects work with boundaries

6. **Create and run unit tests**
   - Implement boundary tests
   - Run tests to verify behavior

7. **Update documentation**
   - Update PLANNING.md if needed
   - Remove references to toroidal topology

## Validation Gates

### Syntax and Style Check
```bash
# Check C syntax and style
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem src/entities/particle.c
cppcheck --enable=all --error-exitcode=1 --suppress=missingIncludeSystem src/entities/managers/particle_manager.c
```

### Compilation Test
```bash
# Clean build to ensure no broken dependencies
make clean
make
```

### Unit Test Execution
```bash
# Compile and run boundary tests
gcc -o test_boundaries tests/unit/test_particle_boundaries.c src/entities/particle.c -lraylib -lm
./test_boundaries
```

### Runtime Validation
```bash
# Run game for smoke test (3 seconds)
timeout 3 ./particle_storm || true

# Check that game starts without crashes
./particle_storm &
GAME_PID=$!
sleep 2
if ps -p $GAME_PID > /dev/null; then
    echo "Game running successfully"
    kill $GAME_PID
else
    echo "Game crashed!"
    exit 1
fi
```

### Performance Check
```bash
# Simple FPS check - game should maintain 60 FPS with boundaries
./test_memory  # Existing performance test
```

## Potential Issues & Solutions

### Issue 1: Particle Clustering at Boundaries
**Problem**: Particles might cluster at edges due to continuous attraction
**Solution**: Already handled by minimum distance check in AttractParticle (0.5f minimum)

### Issue 2: Visual "Jitter" at Boundaries  
**Problem**: Particles might appear to vibrate at edges
**Solution**: Use fabs() to ensure velocity direction is properly set, preventing oscillation

### Issue 3: Performance Impact
**Problem**: More particles near boundaries might affect collision detection
**Solution**: Distance culling already in place, no additional optimization needed

### Issue 4: Backwards Compatibility
**Problem**: Save files or replays might break
**Solution**: N/A - no save system implemented yet

## Anti-Patterns to Avoid
- ❌ Don't allow particles to get stuck outside boundaries
- ❌ Don't use complex physics (just simple bounce)
- ❌ Don't forget to test corner cases (literally!)
- ❌ Don't leave toroidal code commented out - remove it completely
- ❌ Don't change enemy or player boundary behavior

## Files to Modify Summary
1. `src/entities/particle.c` - Core changes to MoveParticle
2. `src/entities/particle.h` - Remove toroidal declarations
3. `src/entities/managers/particle_manager.c` - Update attraction calls
4. `src/core/game.c` - Remove toroidal direction usage
5. `tests/unit/test_particle_boundaries.c` - New test file (create)

## Quality Score: 9/10
**Confidence for one-pass implementation**: Very High
- Clear before/after states
- Reference implementations available (enemy/player)
- Simple physics (bounce = reverse velocity)
- Comprehensive tests provided
- All edge cases considered