# Testing Strategy for Particle Storm

**Created**: 2025-10-21
**Purpose**: Comprehensive testing plan before code refactoring and optimization

## Executive Summary

Based on web research and codebase analysis, this document outlines a testing strategy for the Particle Storm game before refactoring the oversized files (`game.c` 1,263 lines, `enemy.c` 820 lines) and optimizing performance-critical systems.

## Testing Framework

**Current**: MinUnit (391-line lightweight C testing framework)

### MinUnit Capabilities
- ✅ `mu_assert()` - Basic assertions with custom messages
- ✅ `mu_assert_int_eq()` - Integer equality
- ✅ `mu_assert_double_eq()` - Float comparison (epsilon: 1E-12)
- ✅ `mu_assert_string_eq()` - String comparison
- ✅ `mu_check()` - Boolean checks
- ✅ Setup/teardown hooks per test suite
- ✅ Timing metrics (real time + CPU time)
- ✅ Pass/fail statistics

### Current Test Coverage
- **Particle Boundaries** ✅ (158 lines, 7 tests)
- **HP Potion** ✅ (exists)
- **Item Manager** ✅ (exists)
- **Star Item** ✅ (exists)

**Gap Analysis**: Missing tests for core systems (memory pools, physics, AI, events)

---

## Research-Based Testing Principles

### 1. Shift-Left Testing (from web research)
> "QA from early development phases - cost of fixing production bugs is 5-10x higher"

**Application**:
- Write tests BEFORE refactoring game.c and enemy.c
- Validate current behavior to prevent regressions
- Create test harness for new modular architecture

### 2. Memory Pool Validation (from embedded systems research)
Key validation techniques:
- ✅ Null pointer checks in allocate/deallocate
- ✅ Block size and count validation (non-zero)
- ✅ Boundary checking (allocated blocks within pool)
- ✅ Memory leak detection (alloc/dealloc counters)
- ✅ Double-free detection
- ✅ Fragmentation monitoring

### 3. Physics Testing (from simulation research)
Validation strategies:
- ✅ Deterministic behavior with fixed seeds
- ✅ Conservation laws (momentum, energy with friction)
- ✅ Boundary conditions (wall bouncing, corners)
- ✅ Stress tests (100,000 particles)
- ✅ Numerical stability over many iterations

### 4. Game Testing Best Practices (from 2024-2025 trends)
- **Alpha Testing**: Feature completion, unit tests, smoke tests
- **Beta Testing**: Stress testing, edge cases
- **Automated Regression**: Catch issues early
- **Manual Playtesting**: Nuanced gameplay issues

---

## Test Suite Architecture

```
tests/
├── unit/                          # Unit tests (isolated components)
│   ├── test_memory_pool.c         # NEW - Memory pool validation
│   ├── test_physics.c             # NEW - Physics calculations
│   ├── test_collision.c           # NEW - Collision detection
│   ├── test_enemy_init.c          # NEW - Enemy type initialization
│   ├── test_enemy_ai.c            # NEW - AI state transitions
│   ├── test_event_system.c        # NEW - Event pub/sub
│   ├── test_particle_boundaries.c # EXISTS ✓
│   ├── test_hp_potion.c           # EXISTS ✓
│   ├── test_item_manager.c        # EXISTS ✓
│   └── test_star_item.c           # EXISTS ✓
│
├── integration/                   # NEW - Component interactions
│   ├── test_stage_progression.c   # Stage system flow
│   ├── test_particle_enemy_collision.c
│   ├── test_boss_phases.c
│   └── test_game_state_transitions.c
│
└── performance/                   # NEW - Performance benchmarks
    ├── test_100k_particles.c      # 100k particle stress test
    ├── test_collision_performance.c
    └── test_memory_pool_fragmentation.c
```

---

## Priority 1: Core Systems Unit Tests

### 1.1 Memory Pool Tests
**File**: `tests/unit/test_memory_pool.c`

**Test Cases**:
```c
MU_TEST(test_pool_initialization)
    - Create pool with valid parameters
    - Verify capacity, block size, freeCount

MU_TEST(test_pool_allocation)
    - Allocate single block
    - Verify non-null return
    - Verify freeCount decreases

MU_TEST(test_pool_deallocation)
    - Allocate then free block
    - Verify freeCount increases
    - Verify block returns to free list

MU_TEST(test_pool_exhaustion)
    - Allocate all blocks
    - Verify next allocation returns NULL
    - Verify freeCount = 0

MU_TEST(test_pool_null_pointer_safety)
    - Free NULL pointer (should not crash)
    - Allocate from NULL pool (should return NULL)

MU_TEST(test_pool_boundary_checking)
    - Attempt to free block outside pool range
    - Verify error handling

MU_TEST(test_pool_double_free)
    - Free same block twice
    - Verify detection mechanism (if implemented)

MU_TEST(test_pool_stress)
    - Allocate/free 10,000 times randomly
    - Verify no leaks (freeCount matches initial)

MU_TEST(test_pool_fragmentation)
    - Allocate all, free every other block
    - Verify fragmentation metrics
```

**Reason**: Memory pools are critical for 100k particle performance. Must validate before refactoring event system.

---

### 1.2 Physics Tests
**File**: `tests/unit/test_physics.c`

**Test Cases**:
```c
MU_TEST(test_particle_attraction_force)
    - Player at (400, 300), particle at (500, 300)
    - Apply attraction, verify velocity increases toward player
    - Test distance fall-off (inverse square or linear?)

MU_TEST(test_particle_friction)
    - Set velocity to (10, 0)
    - Apply friction (0.99x)
    - Verify velocity = (9.9, 0)
    - After 100 frames, verify significant slowdown

MU_TEST(test_particle_boost_attraction)
    - Normal attraction: 1x
    - Boost attraction: 5x (from PLANNING.md Space key)
    - Verify force multiplier

MU_TEST(test_collision_detection_particle_enemy)
    - Particle at (100, 100), enemy at (101, 100), radius=10
    - Verify collision detected
    - Particle at (120, 100), enemy at (101, 100), radius=10
    - Verify no collision

MU_TEST(test_collision_damage_calculation)
    - Base damage: PARTICLE_ENEMY_DAMAGE = 0.001
    - With shield: verify 0.0005 (50%)
    - Boss enemy: verify 0.0003 (30%)
    - Invulnerable: verify 0.0

MU_TEST(test_batch_collision_processing)
    - 10,000 particles vs 10 enemies
    - Verify all collisions detected
    - Benchmark time (should be < 16ms for 60 FPS)

MU_TEST(test_repulsor_field)
    - Repulsor at (400, 300), radius 150px
    - Particle at (450, 300)
    - Verify velocity reversed and boosted

MU_TEST(test_blackhole_gravity)
    - Blackhole at (400, 300)
    - Particles at various distances
    - Verify attraction strength varies by distance
```

**Reason**: Physics bugs can cascade through gameplay. Validate before refactoring game.c collision code.

---

### 1.3 Collision Detection Tests
**File**: `tests/unit/test_collision.c`

**Test Cases**:
```c
MU_TEST(test_circle_circle_collision_basic)
    - Circle A: (0, 0) radius 10
    - Circle B: (15, 0) radius 10
    - Verify collision (distance = 15 < radii sum 20)

MU_TEST(test_circle_circle_no_collision)
    - Circle A: (0, 0) radius 10
    - Circle B: (25, 0) radius 10
    - Verify no collision (distance = 25 > radii sum 20)

MU_TEST(test_distance_based_culling)
    - Enemy at (0, 0), particle at (1000, 1000)
    - Verify early rejection (distance > threshold)

MU_TEST(test_collision_with_player)
    - Player size: 10px square
    - Enemy at player position + radius
    - Verify collision detection

MU_TEST(test_particle_enemy_collision_event)
    - Trigger collision
    - Verify COLLISION_PARTICLE_ENEMY event published
    - Verify event data contains correct indices
```

**Reason**: Collision is performance bottleneck. Need baseline before optimization.

---

### 1.4 Enemy Initialization Tests
**File**: `tests/unit/test_enemy_init.c`

**Test Cases**:
```c
MU_TEST(test_init_enemy_basic)
    - Create ENEMY_TYPE_BASIC
    - Verify: color=purple, health=radius*10, speed normal

MU_TEST(test_init_enemy_tracker)
    - Create ENEMY_TYPE_TRACKER
    - Verify: color=red, health=radius*12, tracking enabled

MU_TEST(test_init_enemy_speedy)
    - Verify: speed=3x normal, health=radius*8

MU_TEST(test_init_enemy_splitter)
    - Verify: splits_remaining=2, health=radius*15

MU_TEST(test_init_enemy_boss)
    - Verify: size=5x normal, shields enabled, health=500

MU_TEST(test_init_enemy_blackhole)
    - Verify: invulnerable=true, gravity_field enabled

MU_TEST(test_init_enemy_repulsor)
    - Verify: repulsion_radius=150, field enabled

MU_TEST(test_all_enemy_types)
    - Loop through all 11+ enemy types
    - Verify each initializes without crash
    - Verify unique properties set correctly
```

**Reason**: Enemy.c is 820 lines - need validation before splitting into modules.

---

### 1.5 Enemy AI Tests
**File**: `tests/unit/test_enemy_ai.c`

**Test Cases**:
```c
MU_TEST(test_tracker_ai_chase)
    - Tracker far from player
    - Update AI
    - Verify AI_STATE_CHASE, moves toward player

MU_TEST(test_speedy_zigzag_movement)
    - Create speedy enemy
    - Update movement 100 times
    - Verify zigzag pattern (velocity oscillates)

MU_TEST(test_orbiter_circular_movement)
    - Create orbiter with orbit center
    - Update 360 times (1 degree per frame)
    - Verify completes circle

MU_TEST(test_teleporter_timing)
    - Create teleporter
    - Fast-forward timer to teleport threshold
    - Verify position changes

MU_TEST(test_splitter_on_damage)
    - Damage splitter below 50% health
    - Verify splits into 2 enemies
    - Verify splits_remaining decrements

MU_TEST(test_blackhole_invulnerability_phase)
    - Create blackhole with other enemies present
    - Attempt damage
    - Verify health unchanged (invulnerable)

MU_TEST(test_blackhole_vulnerable_phase)
    - Create blackhole, kill all other enemies
    - Wait for vulnerable phase (green)
    - Apply damage
    - Verify health decreases
```

**Reason**: AI is complex and embedded in 820-line file. Tests enable safe refactoring.

---

### 1.6 Event System Tests
**File**: `tests/unit/test_event_system.c`

**Test Cases**:
```c
MU_TEST(test_event_subscribe_publish)
    - Subscribe handler to EVENT_TYPE_TEST
    - Publish event
    - Verify handler called

MU_TEST(test_event_multiple_handlers)
    - Subscribe 3 handlers to same event
    - Publish event
    - Verify all 3 handlers called

MU_TEST(test_event_data_allocation)
    - Allocate event data from pool
    - Verify non-null
    - Publish event with data
    - Verify handler receives correct data

MU_TEST(test_event_data_deallocation)
    - Allocate event data
    - Process event
    - Verify data returned to pool (freeCount increases)

MU_TEST(test_event_queue_overflow)
    - Publish 1025 events (queue size = 1024)
    - Verify oldest event dropped or error

MU_TEST(test_event_unsubscribe)
    - Subscribe handler
    - Unsubscribe handler
    - Publish event
    - Verify handler NOT called

MU_TEST(test_event_handler_error_isolation)
    - Handler 1: crashes/returns error
    - Handler 2: normal
    - Verify handler 2 still executes
```

**Reason**: 59+ event types - must validate pub/sub before adding more complexity.

---

## Priority 2: Integration Tests

### 2.1 Stage Progression Tests
**File**: `tests/integration/test_stage_progression.c`

**Test Cases**:
```c
MU_TEST(test_stage_1_to_stage_2_transition)
    - Complete Stage 1 (kill all enemies)
    - Verify STAGE_COMPLETE event
    - Verify stage number increments
    - Verify Stage 2 enemy composition loaded

MU_TEST(test_spawn_timing_reset)
    - Complete game
    - Start new game
    - Verify ResetSpawnTiming() called
    - Verify enemies spawn in Stage 1

MU_TEST(test_stage_timer_starts_in_intro)
    - Transition to STAGE_INTRO
    - Verify timer > 0 after 1 second
    - Transition to PLAYING
    - Verify timer continues

MU_TEST(test_boss_stage_6)
    - Advance to Stage 6
    - Verify BOSS_1 enemy spawned
    - Verify boss health = 500

MU_TEST(test_boss_stage_10)
    - Advance to Stage 10
    - Verify BOSS_FINAL spawned
    - Verify final boss mechanics active
```

---

### 2.2 Particle-Enemy Collision Integration
**File**: `tests/integration/test_particle_enemy_collision.c`

**Test Cases**:
```c
MU_TEST(test_1000_particles_vs_10_enemies)
    - Spawn 1000 particles, 10 basic enemies
    - Run collision detection
    - Verify damage applied to enemies
    - Verify collision events published

MU_TEST(test_cluster_chain_reaction)
    - Create cluster enemy
    - Kill with particles
    - Verify explosion spawns
    - Verify nearby enemies damaged

MU_TEST(test_splitter_collision_chain)
    - Damage splitter to 50% health
    - Verify splits
    - Continue damaging child splitters
    - Verify 2nd generation splits
```

---

### 2.3 Game State Transition Tests
**File**: `tests/integration/test_game_state_transitions.c`

**Test Cases**:
```c
MU_TEST(test_full_game_loop)
    - TUTORIAL → STAGE_INTRO → PLAYING
    - Complete Stage 1
    - PLAYING → STAGE_COMPLETE → PLAYING (Stage 2)
    - Lose all HP
    - PLAYING → OVER → SCORE_ENTRY → TUTORIAL

MU_TEST(test_pause_unpause)
    - State = PLAYING
    - Press ESC
    - Verify state = PAUSED
    - Verify game logic not updating
    - Press ESC again
    - Verify state = PLAYING
```

---

## Priority 3: Performance Tests

### 3.1 100k Particle Stress Test
**File**: `tests/performance/test_100k_particles.c`

**Test Cases**:
```c
MU_TEST(test_100k_particle_update_time)
    - Create 100,000 particles
    - Update all particles 60 times (1 second)
    - Measure average frame time
    - Assert: frame time < 16ms (60 FPS)

MU_TEST(test_100k_particle_collision_vs_50_enemies)
    - 100k particles, 50 enemies
    - Run collision detection
    - Measure time
    - Assert: < 16ms

MU_TEST(test_memory_pool_100k_allocations)
    - Allocate 100,000 blocks from pool
    - Measure allocation time
    - Free all blocks
    - Verify no leaks
```

---

### 3.2 Memory Pool Fragmentation Test
**File**: `tests/performance/test_memory_pool_fragmentation.c`

**Test Cases**:
```c
MU_TEST(test_long_play_session_simulation)
    - Simulate 10,000 frames (2.7 minutes at 60 FPS)
    - Randomly allocate/free event data
    - Monitor pool utilization
    - Assert: allocation success rate > 95%

MU_TEST(test_worst_case_fragmentation)
    - Allocate all blocks
    - Free every other block (50% fragmentation)
    - Attempt large allocation
    - Measure success/failure
```

---

## Test Execution Strategy

### Build System Integration

Add to `Makefile`:
```makefile
# Test targets
test: test_unit test_integration test_performance

test_unit:
	@echo "Running unit tests..."
	@./tests/unit/test_memory_pool
	@./tests/unit/test_physics
	@./tests/unit/test_collision
	@./tests/unit/test_enemy_init
	@./tests/unit/test_enemy_ai
	@./tests/unit/test_event_system
	@./tests/unit/test_particle_boundaries
	@./tests/unit/test_hp_potion
	@./tests/unit/test_item_manager
	@./tests/unit/test_star_item

test_integration:
	@echo "Running integration tests..."
	@./tests/integration/test_stage_progression
	@./tests/integration/test_particle_enemy_collision
	@./tests/integration/test_game_state_transitions

test_performance:
	@echo "Running performance tests..."
	@./tests/performance/test_100k_particles
	@./tests/performance/test_memory_pool_fragmentation
```

### Pre-Refactoring Workflow

1. **Baseline Tests** (Week 1)
   - Write all Priority 1 unit tests
   - Run tests, fix any failures
   - Document current behavior

2. **Integration Tests** (Week 2)
   - Write Priority 2 integration tests
   - Validate system interactions
   - Identify coupling issues

3. **Performance Baseline** (Week 3)
   - Write Priority 3 performance tests
   - Establish performance benchmarks
   - Profile bottlenecks

4. **Refactoring** (Week 4+)
   - Split game.c into modules
   - Split enemy.c into modules
   - Re-run all tests after each refactoring step
   - Verify no regressions

---

## Success Criteria

### Code Coverage Goals
- **Unit Tests**: 80% coverage of core systems
- **Integration Tests**: All state transitions covered
- **Performance Tests**: All bottlenecks benchmarked

### Performance Targets
- 100k particles @ 60 FPS (< 16ms per frame)
- Collision detection < 8ms per frame
- Event processing < 2ms per frame
- Memory pool allocation < 1μs per allocation

### Quality Gates
Before refactoring:
- ✅ All unit tests pass (0 failures)
- ✅ All integration tests pass
- ✅ Performance tests meet targets
- ✅ No memory leaks detected
- ✅ Static analysis clean (cppcheck)

---

## Tools and Resources

### Static Analysis
- **cppcheck**: Static code analysis
  ```bash
  cppcheck --enable=all src/
  ```

### Memory Analysis
- **Valgrind** (Linux/macOS): Memory leak detection
  ```bash
  valgrind --leak-check=full ./bin/game
  ```

### Performance Profiling
- **gprof**: GNU profiler
  ```bash
  gcc -pg ...
  gprof bin/game gmon.out
  ```

- **perf** (Linux): CPU profiling
  ```bash
  perf record ./bin/game
  perf report
  ```

---

## Testing Best Practices (From Research)

### 1. Test Isolation
- Each test is independent
- No shared state between tests
- Use setup/teardown for clean environment

### 2. Fast Execution
- Unit tests should run in milliseconds
- Entire suite < 5 seconds for rapid iteration

### 3. Clear Failure Messages
```c
mu_assert(health > 0, "Enemy health must be positive after init");
```

### 4. Deterministic Tests
- Use fixed random seeds for reproducibility
- Avoid timing-dependent tests

### 5. Test Naming Convention
```c
test_<system>_<scenario>_<expected_result>
test_enemy_init_boss_has_shields()
test_physics_particle_bounce_reverses_velocity()
```

---

## Risk Mitigation

### Risk 1: Tests break during refactoring
**Mitigation**:
- Write tests against public API, not implementation
- Use interface abstraction
- Refactor in small, testable increments

### Risk 2: Performance regressions undetected
**Mitigation**:
- Run performance tests on every major change
- Set up CI/CD to auto-run tests
- Track metrics over time (spreadsheet/graph)

### Risk 3: Test maintenance burden
**Mitigation**:
- Keep tests simple and focused
- Avoid duplicate test logic
- Use helper functions for common setup

---

## Next Steps

1. ✅ **Create this document** (TESTING_STRATEGY.md)
2. ⏳ **Implement Priority 1 tests** (estimated: 3-4 days)
   - test_memory_pool.c
   - test_physics.c
   - test_collision.c
   - test_enemy_init.c
   - test_enemy_ai.c
   - test_event_system.c

3. ⏳ **Run baseline tests** (1 day)
   - Fix any failures
   - Document current behavior

4. ⏳ **Implement Priority 2 tests** (2-3 days)
   - Integration tests

5. ⏳ **Implement Priority 3 tests** (1-2 days)
   - Performance benchmarks

6. ⏳ **Begin refactoring** (ongoing)
   - Split game.c
   - Split enemy.c
   - Re-test continuously

---

## References

- Web Research: Game testing best practices 2024-2025
- Web Research: Memory pool validation techniques
- Web Research: Particle simulation testing
- Existing Codebase: test_particle_boundaries.c (good example)
- MinUnit Documentation: src/minunit/minunit.h

---

**Document Status**: Draft
**Last Updated**: 2025-10-21
**Owner**: Development Team
**Review Date**: After Priority 1 tests complete
