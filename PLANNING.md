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

# Test a specific stage (jumps directly to that stage)
make test-stage-1      # Test Stage 1: Gravitational Anomaly
make test-stage-6      # Test Stage 6: Boss Battle
make test-stage-10     # Test Stage 10: Final Boss

# Launch developer test mode (interactive enemy testing)
make test-enemy        # Spawns enemies interactively with mouse/keyboard

# Compile a single stage file for validation
make compile-stage-3   # Check Stage 3 for compilation errors

# Basic runtime test (runs game for 3 seconds)
./test_memory
```

## Project Overview

**Particle Storm** is a physics-based particle simulation game built with Raylib 5.5, featuring 100,000 particles, event-driven architecture, and 10 progressively challenging stages with unique enemy types and boss battles.

- Raylib context7 latest doc: `upstash-context-7-mcp get-library-docs /raysan5/raylib`

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
- `src/entities/managers/stage_manager.c` - Stage progression core logic
- `src/entities/managers/stage_manager.h` - Stage system interface
- `src/entities/managers/stages/stage_common.h` - Common interface for all stages
- `src/entities/managers/stages/stage_N.c` - Individual stage definitions (N = 1-10)

**Stage Modularity:**
Each stage is now defined in its own file under `src/entities/managers/stages/`. This provides:
- **Independent Development**: Modify one stage without affecting others
- **Version Control**: Track changes per-stage with Git
- **Easy Testing**: Test individual stages with `make test-stage-N`
- **Clean Code**: Each file is under 200 lines, well-documented

**Important Notes:**
- Static variables in `ShouldSpawnEnemy()` need reset when restarting (use `ResetSpawnTiming()`)
- Stage timer starts during STAGE_INTRO phase
- Each stage has unique enemy composition and wave patterns
- All stages must implement `Stage CreateStageN(void)` function

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
Normal Flow:
GAME_STATE_TUTORIAL → GAME_STATE_STAGE_INTRO → GAME_STATE_PLAYING → GAME_STATE_OVER → GAME_STATE_SCORE_ENTRY → (back to TUTORIAL)

Developer Mode:
GAME_STATE_TEST_MODE (launched with --test-mode flag)
  - Interactive enemy spawning/removal
  - No win/loss conditions
  - Direct enemy type selection
  - ESC to exit back to tutorial
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

### Developer Test Mode
Interactive environment for testing individual enemy types without modifying code:

**Launch Test Mode:**
```bash
# Desktop
make test-enemy
./bin/game --test-mode

# Web (select "Developer Test Mode" from dropdown)
http://localhost:8000/particle_storm.html?test-mode=1
```

**Test Mode Controls:**
- **Left Click**: Spawn selected enemy at mouse cursor
- **TAB**: Cycle to next enemy type (supports unlimited enemies)
- **Shift+TAB**: Cycle to previous enemy type
- **1-9, 0**: Quick select first 10 enemy types (1=BASIC through 0=CLUSTER)
- **R**: Remove nearest enemy to cursor
- **C**: Clear all enemies
- **F1**: Toggle help overlay
- **ESC**: Exit test mode, return to main menu

**Test Mode Features:**
- Clean environment (space grey background, white particles)
- No automatic enemy spawning
- No win/loss conditions
- No time limits
- Up to 50 enemies can be spawned
- Real-time statistics display (enemies spawned/removed)
- **Supports unlimited enemy types** - TAB cycling works with any number of enemies
- Currently 11 enemy types available (BASIC through BLACKHOLE)

**Key Files:**
- `src/core/dev_test_mode.c/h` - Test mode logic
- `src/entities/managers/stages/stage_test.c` - Test mode stage configuration

**Adding New Enemy Types to Test Mode:**
When you add a new enemy type, update these 2 locations:

1. **`src/entities/enemy.h`** - Add to EnemyType enum (before ENEMY_TYPE_COUNT)
2. **`src/core/dev_test_mode.c:8`** - Add name to ENEMY_TYPE_NAMES array

The test mode will automatically:
- Include new enemy in TAB cycling
- Support spawning via mouse click
- Display enemy name in UI
- Allow removal and clearing

No other changes needed! TAB/Shift+TAB provides access to all enemy types, regardless of count.

### Testing Specific Enemies (Legacy Method)
To test specific enemy types by modifying stage definitions:
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
│   ├── dev_test_mode.c/h    # Developer test mode (NEW)
│   └── event/
│       ├── event_system.c/h # Event system core
│       └── event_types.h    # Event structures
└── entities/
    ├── player.c/h           # Player logic
    ├── enemy.c/h            # Enemy base implementation
    ├── particle.c/h         # Particle simulation
    ├── explosion.c/h        # Explosion effects
    ├── items/
    │   ├── hp_potion.c/h    # HP potion item
    │   └── star_item.c/h    # Star collection item
    └── managers/
        ├── enemy_manager.c/h    # Enemy spawning/AI
        ├── particle_manager.c/h # Particle management
        ├── item_manager.c/h     # Item spawning/collection
        ├── stage_manager.c/h    # Stage progression core
        └── stages/              # Individual stage definitions
            ├── stage_common.h   # Common stage interface
            ├── stage_1.c        # Stage 1: Gravitational Anomaly
            ├── stage_2.c        # Stage 2: The Hunt Begins
            ├── stage_3.c        # Stage 3: Speed Demons
            ├── stage_4.c        # Stage 4: Divide and Conquer
            ├── stage_5.c        # Stage 5: Orbital Mechanics
            ├── stage_6.c        # Stage 6: Guardian (Boss)
            ├── stage_7.c        # Stage 7: Quantum Flux
            ├── stage_8.c        # Stage 8: Magnetic Storm
            ├── stage_9.c        # Stage 9: Chain Reaction
            ├── stage_10.c       # Stage 10: Final Boss
            └── stage_test.c     # Developer test mode stage (NEW)
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

## Stage Development Workflow

### Working on Stages

Each stage can be developed independently following this workflow:

1. **Create a feature branch:**
   ```bash
   git checkout -b stage/stage-3-balance-update
   ```

2. **Edit the stage file:**
   ```bash
   # Edit src/entities/managers/stages/stage_3.c
   ```

3. **Test your changes:**
   ```bash
   make compile-stage-3  # Check compilation
   make test-stage-3     # Playtest your stage
   ```

4. **Commit with clear messages:**
   ```bash
   git commit -m "Stage 3: Reduce speedy enemy health from 1.2x to 1.0x

   The stage was too difficult for new players based on playtesting.
   This change makes the fast enemies easier to destroy while
   maintaining the speed challenge."
   ```

5. **Push and create PR:**
   ```bash
   git push origin stage/stage-3-balance-update
   ```

### Documentation

See detailed guides in `.github/`:
- **STAGE_WORKFLOW.md**: Git branching strategy and PR process
- **STAGE_DEVELOPMENT.md**: Complete guide to creating/modifying stages

### Quick Reference

```bash
# Test a stage
make test-stage-N

# Compile just one stage
make compile-stage-N

# Build everything
make clean && make

# Run full game
make run
```

## Contributing

When contributing to this project:
1. Follow the existing code style
2. Add appropriate event handlers for new features
3. Use memory pools for dynamic allocations
4. Test with 100,000 particles for performance
5. For stage changes, see `.github/STAGE_WORKFLOW.md`
6. Update this documentation for significant changes