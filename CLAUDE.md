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

## Debugging Commands

When debugging issues after game restart or stage transitions:
```bash
# Run with debug output (already implemented in game.c)
./bin/game
```

## Architecture Overview

This is a particle-based game built with Raylib featuring an event-driven architecture. The game uses 100,000 particles that interact with enemies through physics simulation.

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