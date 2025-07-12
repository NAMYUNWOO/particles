# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Clean and build the project
make clean
make

# Run the game
make run

# Build and run in one command
make clean && make && make run
```

## Development Commands

```bash
# Run with debug output (shows stage info every 2 seconds)
./bin/game

# Basic runtime test (runs game for 3 seconds)
./test_memory
```

## Architecture Overview

This is a particle-based game built with Raylib 5.5 featuring an event-driven architecture with component-based design. The game uses 100,000 particles that interact with enemies through physics simulation.

### Core Systems

1. **Event System**: The game uses a publish-subscribe event system for decoupling components. All major interactions (collisions, input, state changes) go through events. Key files:
   - `src/core/event/event_system.c` - Core event handling
   - `src/core/event/event_types.h` - Event data structures
   - Memory pools are used for event data to prevent allocation overhead

2. **Stage System**: The game has 10 stages with different enemy types and mechanics:
   - `src/entities/managers/stage_manager.c` - Stage definitions and progression
   - Static variables in `ShouldSpawnEnemy()` need reset when restarting (use `ResetSpawnTiming()`)
   - Stage timer starts during STAGE_INTRO phase

3. **Memory Pool System**: Critical for performance with 100,000 particles:
   - `src/core/memory_pool.c` - Pool implementation
   - Used for: event data, collision data, enemy events, explosion particles
   - Prevents malloc/free overhead during gameplay

4. **Physics System**: Handles particle attraction and collision:
   - Player attracts particles within range
   - Particles damage enemies on collision
   - Batch collision processing for performance

### Key Game States

```
GAME_STATE_TUTORIAL → GAME_STATE_STAGE_INTRO → GAME_STATE_PLAYING → GAME_STATE_OVER → GAME_STATE_SCORE_ENTRY → (back to TUTORIAL)
```

### Enemy Types

- **ENEMY_TYPE_BLACKHOLE**: Special enemy with gravity/magnetic storm mechanics
  - Invulnerable while other enemies exist
  - Transforms after all enemies die: pulse → semi-magnetic storm
  - 5 seconds storm (red), 1 second vulnerable (green)

### Common Issues & Solutions

1. **Enemies not spawning after game restart**: 
   - Check `ResetSpawnTiming()` is called when transitioning from TUTORIAL to STAGE_INTRO
   - Ensure `currentStageNumber` is reset to 0 after game over

2. **Performance issues with 100,000 particles**:
   - Memory pools must be properly initialized
   - Batch collision processing is essential
   - Distance-based culling for physics calculations

3. **Event system memory leaks**:
   - All event data must be allocated from pools
   - Event handlers must properly free event data after processing

### Testing Enemy Behaviors

To test specific enemy types, modify stage definitions in `stage_manager.c`. For example, to test blackhole enemy:
- Edit `CreateStage1()` to spawn blackhole enemies
- Blackhole config: health=1000, tracking movement, invulnerable initially

### Performance Monitoring

The game includes debug output that prints every 2 seconds during gameplay:
- Stage number, timer, enemy count, stage state, current wave
- Useful for diagnosing spawn issues

## Entity System

### Entity Types and Behaviors

1. **Player** (`src/entities/player.h`):
   - Boost system with gauge (speed boost, particle attraction boost)
   - Invincibility frames after damage
   - Toroidal world movement

2. **Particles** (`src/entities/particle.h`):
   - 100,000 pre-allocated instances
   - Attracted to player within range
   - Damage enemies on collision (1 damage per particle)

3. **Enemies** (`src/entities/enemy.h`) - 11 types:
   - BASIC: Simple movement patterns
   - CHARGER: High-speed direct attacks
   - SPLITTER: Splits into smaller enemies when damaged
   - TELEPORTER: Random teleportation
   - REPULSOR: Pushes particles away
   - BLACKHOLE: Complex boss with magnetic storm phases
   - SHIELD_GENERATOR: Creates protective barriers
   - BOMBER: Explosive attacks
   - CLUSTER: Groups that move together
   - SNAKE_HEAD/SEGMENT: Chain enemies
   - NIGHTMARE_LORD: Final boss (see boss_nightmare_design.md)

### Critical Implementation Notes

1. **Memory Management**:
   - Always use memory pools for event data allocation
   - Never allocate event data with malloc during gameplay
   - Event handlers must free event data after processing

2. **State Transitions**:
   - Reset spawn timing when transitioning from TUTORIAL to STAGE_INTRO
   - Stage timer starts during STAGE_INTRO phase, not PLAYING
   - Current stage number must reset to 0 after game over

3. **Collision Processing**:
   - Batch process collisions for performance
   - Use distance-based culling before detailed checks
   - Collision events are pooled to prevent allocation

4. **Event System Toggle**:
   - Can be disabled for performance testing
   - When disabled, systems must handle direct calls

## Platform-Specific Notes

- **macOS**: Raylib installed via Homebrew at `/opt/homebrew/Cellar/raylib/5.5`
- **Windows**: Uses W64Devkit with Raylib at `C:/Users/namyunwoo/W64Devkit/w64devkit`