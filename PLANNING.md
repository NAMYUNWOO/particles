# PLANNING.md - Particle Storm Game

## Build Commands

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

## Project Overview

**Particle Storm** is a physics-based particle simulation game built with Raylib 5.5, featuring 100,000 particles, event-driven architecture, and 10 progressively challenging stages with unique enemy types and boss battles.

### Core Features
- **100,000 Particles**: Real-time physics simulation with attraction mechanics
- **Event-Driven Architecture**: Decoupled components using publish-subscribe pattern
- **Memory Pool Optimization**: Efficient memory management for high performance

## Architecture Overview

### 1. Event System
The game uses a publish-subscribe event system for component decoupling:

**Key Files:**
- `src/core/event/event_system.c` - Core event handling
- `src/core/event/event_types.h` - Event data structures

**Features:**
- Memory pools for event data to prevent allocation overhead
- All major interactions (collisions, input, state changes) go through events
- Multiple systems can subscribe to the same event with different handlers

### 2. Stage System
10 stages with different enemy types and mechanics:

**Key Files:**
- `src/entities/managers/stage_manager.c` - Stage definitions and progression
- `src/entities/managers/stage_manager.h` - Stage system interface

**Important Notes:**
- Static variables in `ShouldSpawnEnemy()` need reset when restarting (use `ResetSpawnTiming()`)
- Stage timer starts during STAGE_INTRO phase
- Each stage has unique enemy composition and wave patterns

### 3. Memory Pool System
Critical for performance with 100,000 particles:

**Key Files:**
- `src/core/memory_pool.c` - Pool implementation
- `src/core/memory_pool.h` - Pool interface

**Used For:**
- Event data
- Collision data
- Enemy events
- Explosion particles
- Prevents malloc/free overhead during gameplay

### 4. Physics System
Handles particle attraction and collision:
- Player attracts particles within range
- Particles damage enemies on collision (1 damage per particle)
- Batch collision processing for performance
- Distance-based culling for optimization

## Game States

```
GAME_STATE_TUTORIAL → GAME_STATE_STAGE_INTRO → GAME_STATE_PLAYING → GAME_STATE_OVER → GAME_STATE_SCORE_ENTRY → (back to TUTORIAL)
```

## Stage Design
Stage Design is written in `STAGE_DESIGN.md` .

## Enemy Types Reference

| Enemy Type | Symbol | Color | Special Ability | Base Score | Health |
|------------|--------|-------|-----------------|------------|--------|
| BASIC | - | Purple | None | 100 | 50 |
| TRACKER | T | Red | Tracks player | 150 | 75 |
| SPEEDY | S | Sky Blue | High speed, zigzag | 200 | 40 |
| SPLITTER | X | Green | Splits when damaged | 250 | 100 |
| ORBITER | O | Orange | Circular orbit movement | 180 | 60 |
| BOSS_1 | B1 | Dark Purple | Shields, phase changes | 1000 | 500 |
| TELEPORTER | ! | Purple | Teleportation | 300 | 80 |
| REPULSOR | R | Yellow | Particle repulsion field | 350 | 90 |
| CLUSTER | C | Magenta | Chain explosions | 220 | 70 |
| BLACKHOLE | BH | Black/Red | Gravity storm, invulnerability phases | 400 | 1000 |
| SHIELD_GENERATOR | SG | Cyan | Creates barriers | 200 | 150 |
| BOMBER | B | Orange/Red | Explosive attacks | 280 | 120 |
| SNAKE_HEAD | SH | Green | Chain movement | 300 | 200 |
| SNAKE_SEGMENT | SS | Light Green | Follows head | 50 | 50 |
| NIGHTMARE_LORD | NL | Dark Red | Final boss abilities | 2000 | 1000 |
| FINAL_BOSS | BF | Golden | All abilities | 2000 | 1000 |

### Special Enemy: BLACKHOLE
- Invulnerable while other enemies exist
- Transforms after all enemies die: pulse → semi-magnetic storm
- 5 seconds storm phase (red), 1 second vulnerable phase (green)
- Requires strategic timing to defeat

## Common Issues & Solutions

### 1. Enemies not spawning after game restart
- **Solution**: Ensure `ResetSpawnTiming()` is called when transitioning from TUTORIAL to STAGE_INTRO
- **Check**: `currentStageNumber` is reset to 0 after game over

### 2. Performance issues with 100,000 particles
- **Solution**: Verify memory pools are properly initialized
- **Optimization**: Use batch collision processing
- **Technique**: Implement distance-based culling for physics calculations

### 3. Event system memory leaks
- **Prevention**: All event data must be allocated from pools
- **Cleanup**: Event handlers must properly free event data after processing
- **Monitoring**: Check memory pool usage regularly

### 4. Stage progression bugs
- **Issue**: Timer not starting correctly
- **Solution**: Ensure timer starts during STAGE_INTRO phase, not PLAYING

## Testing & Debugging

### Debug Output
The game prints debug info every 2 seconds during gameplay:
- Stage number
- Stage timer
- Enemy count
- Stage state
- Current wave
- Memory pool usage (if enabled)

### Developer Keys
- `F1`: Show current stage information
- `F2`: Skip to next stage (development only)
- `F3`: Toggle invincibility mode (development only)
- `F4`: Toggle debug overlay
- `F5`: Reload current stage

### Testing Specific Enemies
To test specific enemy types, modify stage definitions in `stage_manager.c`:
```c
// Example: Test blackhole enemy in Stage 1
void CreateStage1() {
    // Edit enemy composition
    stage.enemyTypes[0] = ENEMY_TYPE_BLACKHOLE;
    stage.enemyCounts[0] = 1;
    // Blackhole config: health=1000, tracking movement, invulnerable initially
}
```

## Controls

- **WASD/Arrow Keys**: Move player
- **Space**: Hold to increase particle attraction (2x strength, uses gauge)
- **Left Shift**: Hold to increase movement speed (2x speed, uses gauge)
- **Enter**: Progress through menus/stages
- **ESC**: Pause game

## File Structure

```
src/
├── main.c                    # Entry point, game loop
├── core/
│   ├── game.c/h             # Game state management
│   ├── physics.c/h          # Physics simulation
│   ├── input_handler.c/h    # Input processing
│   ├── memory_pool.c/h      # Memory pool system
│   └── event/
│       ├── event_system.c/h # Event system core
│       └── event_types.h    # Event structures
└── entities/
    ├── player.c/h           # Player logic
    ├── enemy.c/h            # Enemy base implementation
    ├── particle.c/h         # Particle simulation
    ├── explosion.c/h        # Explosion effects
    └── managers/
        ├── enemy_manager.c/h    # Enemy spawning/AI
        ├── particle_manager.c/h # Particle management
        └── stage_manager.c/h    # Stage progression
```

## Performance Monitoring

### Key Metrics
- FPS (target: 60)
- Particle update time
- Collision detection time
- Event processing time
- Memory pool utilization

### Optimization Strategies
1. **Spatial Partitioning**: Divide world into grid for collision detection
2. **LOD System**: Reduce particle detail at distance
3. **Event Batching**: Process similar events together
4. **Culling**: Skip off-screen entities

## Critical Implementation Notes

### Memory Management
- Always use memory pools for event data allocation
- Never allocate event data with malloc during gameplay
- Event handlers must free event data after processing
- Monitor pool fragmentation

### State Transitions
- Reset spawn timing when transitioning from TUTORIAL to STAGE_INTRO
- Stage timer starts during STAGE_INTRO phase, not PLAYING
- Current stage number must reset to 0 after game over
- Save game state before boss battles

### Collision Processing
- Batch process collisions for performance
- Use distance-based culling before detailed checks
- Collision events are pooled to prevent allocation
- Priority system for important collisions

### Event System Toggle
- Can be disabled for performance testing
- When disabled, systems must handle direct calls
- Useful for profiling specific subsystems

## Score System

### Score Calculation
- Base score + (stage number × 10%)
- Stage clear bonus: 500 × stage number
- Chain explosion bonus
- Boss defeat bonus
- Perfect stage bonus (no damage taken)

### Leaderboard
- Top 10 scores saved to `scoreboard.txt`
- Name entry after game over
- Stage reached displayed with score

## Future Development

### Planned Features
- Endless mode after completing all stages
- Co-op multiplayer support
- Power-up system (shields, weapon upgrades)
- Achievement system
- Online leaderboard synchronization
- Custom stage editor
- Particle color customization
- Sound effects and music

### Technical Improvements
- Vulkan/Metal rendering backend
- Multi-threading for physics
- Network play support
- Replay system
- Mod support

## Known Issues

1. **Particle clustering**: Under certain conditions, particles may cluster unnaturally
2. **Boss phase transitions**: Occasional visual glitches during phase changes
3. **Memory pool fragmentation**: Long play sessions may experience degraded performance
4. **Teleporter timing**: Teleport animations sometimes overlap

## Contributing

When contributing to this project:
1. Follow the existing code style
2. Add appropriate event handlers for new features
3. Use memory pools for dynamic allocations
4. Test with 100,000 particles for performance
5. Update this documentation for significant changes