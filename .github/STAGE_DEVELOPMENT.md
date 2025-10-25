# Stage Development Guide

Complete guide for creating and modifying game stages in Particle Storm.

## Table of Contents

1. [Stage File Structure](#stage-file-structure)
2. [Creating a New Stage](#creating-a-new-stage)
3. [Stage Configuration Reference](#stage-configuration-reference)
4. [Enemy Wave Design](#enemy-wave-design)
5. [Testing Your Stage](#testing-your-stage)
6. [Balancing Guidelines](#balancing-guidelines)
7. [Common Patterns](#common-patterns)
8. [Troubleshooting](#troubleshooting)

## Stage File Structure

Each stage is defined in its own file: `src/entities/managers/stages/stage_N.c`

### Basic Template

```c
/**
 * @file stage_N.c
 * @brief Stage N: <Stage Name>
 *
 * <Brief description of stage mechanics and theme>
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage N configuration
 *
 * Enemy composition:
 * - Wave 1 (Xs): <enemy description>
 * - Wave 2 (Xs): <enemy description>
 * - Wave N (Xs): <enemy description>
 *
 * Special mechanics:
 * - <mechanic 1>
 * - <mechanic 2>
 *
 * @return Stage struct with all configuration
 */
Stage CreateStageN(void) {
    Stage stage = {0};
    stage.stageNumber = N;
    strcpy(stage.name, "Stage Name");
    strcpy(stage.description, "Brief player-facing description");

    // Configure waves
    stage.waveCount = X;

    // Wave definitions...

    // Stage parameters
    stage.targetKills = XX;
    stage.maxEnemiesAlive = X;
    stage.enemyHealthMultiplier = X.Xf;
    stage.enemySpeedMultiplier = X.Xf;
    stage.enemySizeMultiplier = X.Xf;
    stage.backgroundColor = (Color){R, G, B, 255};
    stage.particleColor = COLOR_NAME;

    return stage;
}
```

## Creating a New Stage

### Step 1: Create the File

```bash
# Create the new stage file
touch src/entities/managers/stages/stage_N.c

# Add it to Makefile
# Edit Makefile and add: $(STAGES_DIR)/stage_N.c \
```

### Step 2: Declare in stage_common.h

```c
// Add to src/entities/managers/stages/stage_common.h
Stage CreateStageN(void);
```

### Step 3: Implement the Stage

Use the template above and configure according to design.

### Step 4: Add Makefile Target

```makefile
# Add to Makefile
test-stage-N: all
	@echo "Testing Stage N: <Stage Name>"
	@./$(BIN_DIR)/game --start-stage N
```

## Stage Configuration Reference

### Core Parameters

| Parameter | Type | Description | Typical Range |
|-----------|------|-------------|---------------|
| `stageNumber` | int | Stage number (1-10) | 1-10 |
| `name` | char[64] | Display name | Max 63 chars |
| `description` | char[256] | Player hint | Max 255 chars |
| `waveCount` | int | Number of enemy waves | 2-5 |
| `targetKills` | int | Enemies to defeat | 5-25 |
| `maxEnemiesAlive` | int | Max simultaneous enemies | 5-10 |

### Multipliers

| Multiplier | Description | Balance Guidelines |
|------------|-------------|-------------------|
| `enemyHealthMultiplier` | Enemy HP modifier | Early: 1.0-1.3, Mid: 1.3-1.7, Late: 1.7-2.5, Boss: 2.0-3.0 |
| `enemySpeedMultiplier` | Enemy speed modifier | Slow: 0.7-0.9, Normal: 1.0-1.2, Fast: 1.3-1.8 |
| `enemySizeMultiplier` | Enemy size modifier | Small: 0.8-0.9, Normal: 1.0, Large: 1.2-1.5, Boss: 1.5-2.5 |
| `particleAttractionMultiplier` | Particle control | Easy: 1.2-1.5, Normal: 1.0, Hard: 0.7-0.9 |

### Visual Theme

```c
// Example color schemes
stage.backgroundColor = (Color){20, 20, 30, 255};    // Dark space
stage.backgroundColor = (Color){255, 240, 200, 255};  // Warm orange
stage.backgroundColor = BLACK;                         // Boss battle

stage.particleColor = BLUE;           // Standard
stage.particleColor = RED;            // Dangerous
stage.particleColor = GOLD;           // Final boss
stage.particleColor = (Color){R, G, B, 255};  // Custom
```

### Special Flags

```c
stage.hasSpecialEffect = true;  // Enable for boss stages (6, 10)
```

## Enemy Wave Design

### Wave Structure

```c
stage.waves[waveIndex] = (EnemyWave){
    .spawnTime = X.Xf,          // When wave starts (seconds from stage start)
    .enemyCount = X,             // Number of enemies in this wave
    .enemyTypes = {TYPE1, TYPE2, ...},  // Array of enemy types
    .spawnDelay = X.Xf,          // Delay between enemy spawns (seconds)
    .useRandomSpawn = true/false,  // Random or pattern-based spawning
    .spawnPattern = {{x1, y1}, {x2, y2}, ...}  // If !useRandomSpawn
};
```

### Enemy Types Available

| Type | Name | Characteristics | Best Used For |
|------|------|-----------------|---------------|
| `ENEMY_TYPE_BASIC` | Basic | Simple movement | Early waves, filler |
| `ENEMY_TYPE_TRACKER` | Tracker | Follows player | Teaching dodging |
| `ENEMY_TYPE_SPEEDY` | Speedy | Fast zigzag | Quick reflexes |
| `ENEMY_TYPE_SPLITTER` | Splitter | Splits on damage | Crowd control |
| `ENEMY_TYPE_ORBITER` | Orbiter | Circular patterns | Pattern recognition |
| `ENEMY_TYPE_BOSS_1` | Boss 1 | Shields, phases | Mid-game climax |
| `ENEMY_TYPE_TELEPORTER` | Teleporter | Unpredictable | Advanced challenge |
| `ENEMY_TYPE_REPULSOR` | Repulsor | Pushes particles | Anti-strategy |
| `ENEMY_TYPE_CLUSTER` | Cluster | Chain explosions | Chaos management |
| `ENEMY_TYPE_BLACKHOLE` | Blackhole | Gravity mechanics | Special mechanics |
| `ENEMY_TYPE_BOSS_FINAL` | Final Boss | All abilities | Final challenge |

### Wave Timing Guidelines

```c
// Example progression
stage.waves[0].spawnTime = 0.0f;     // Immediate start
stage.waves[1].spawnTime = 8.0f;     // After first wave cleared
stage.waves[2].spawnTime = 16.0f;    // Build tension
stage.waves[3].spawnTime = 25.0f;    // Final challenge
```

**Rules of Thumb:**
- First wave: 0.0s (immediate)
- Subsequent waves: 6-10s apart
- Boss appearance: 2-3s delay for dramatic effect
- Final wave: After 25-35s for climactic finish

### Spawn Patterns

#### Random Spawn (Most Common)
```c
.useRandomSpawn = true
// Enemies spawn anywhere on screen edges
```

#### Pattern Spawn (Boss/Special)
```c
.useRandomSpawn = false,
.spawnPattern = {
    {400, 200},   // Center-top
    {200, 400},   // Left-center
    {600, 400}    // Right-center
}
```

**Common Patterns:**
```c
// Boss center spawn
.spawnPattern = {{400, 300}}

// Corner formation
.spawnPattern = {{100, 100}, {700, 100}, {100, 700}, {700, 700}}

// Horizontal line
.spawnPattern = {{200, 400}, {400, 400}, {600, 400}}
```

## Testing Your Stage

### 1. Compilation Test
```bash
make compile-stage-N
```
Should output: `Stage N compiled successfully`

### 2. Isolated Stage Test
```bash
make test-stage-N
```
This will:
- Build the game
- Jump directly to your stage
- Allow you to playtest without completing previous stages

### 3. Full Integration Test
```bash
make clean && make run
```
Play from Stage 1 to reach your stage naturally.

### 4. Validation Checklist

- [ ] Stage compiles without warnings
- [ ] All waves spawn at correct times
- [ ] Enemy count matches `targetKills`
- [ ] Stage completes when reaching target kills
- [ ] Visual theme is distinct and appropriate
- [ ] Difficulty feels right for stage position
- [ ] Transitions smoothly to next stage
- [ ] No crashes or unexpected behavior

## Balancing Guidelines

### Stage Difficulty Curve

| Stage | Difficulty | Focus | Target Duration |
|-------|-----------|-------|-----------------|
| 1 | Tutorial | Learn mechanics | 30-40s |
| 2-3 | Easy | Basic enemies | 30-45s |
| 4-5 | Medium | Mixed enemies | 40-50s |
| 6 | Boss | First major challenge | 60-90s |
| 7-8 | Hard | Advanced mechanics | 50-60s |
| 9 | Very Hard | Chaos management | 60-70s |
| 10 | Final Boss | Ultimate test | 90-120s |

### Balancing Formula

```
Expected Playtime (seconds) = (targetKills × 3) + (waveCount × 5)

Example Stage 1:
(9 kills × 3) + (3 waves × 5) = 27 + 15 = 42 seconds ✓
```

### Testing Balance

Playtest and check:
1. **Too Easy**: Player never feels threatened
   - Solution: Increase enemy speed/health or reduce spawn delays

2. **Too Hard**: Player dies repeatedly in first 10 seconds
   - Solution: Reduce enemy count, increase spawn delays, or lower health

3. **Too Long**: Stage drags beyond 90s (non-boss)
   - Solution: Reduce target kills or increase enemy spawn rate

4. **Too Short**: Completed in under 20s
   - Solution: Increase target kills or add waves

## Common Patterns

### Pattern 1: Gradual Introduction
```c
// Wave 1: Introduce new enemy type with familiar enemies
stage.waves[0] = (EnemyWave){
    .enemyTypes = {NEW_TYPE, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC}
};

// Wave 2: More of the new type
stage.waves[1] = (EnemyWave){
    .enemyTypes = {NEW_TYPE, NEW_TYPE, ENEMY_TYPE_TRACKER}
};

// Wave 3: Pure new type challenge
stage.waves[2] = (EnemyWave){
    .enemyTypes = {NEW_TYPE, NEW_TYPE, NEW_TYPE}
};
```

### Pattern 2: Boss Stage
```c
stage.waveCount = 2;

// Boss spawn with dramatic delay
stage.waves[0] = (EnemyWave){
    .spawnTime = 2.0f,
    .enemyCount = 1,
    .enemyTypes = {ENEMY_TYPE_BOSS_1},
    .useRandomSpawn = false,
    .spawnPattern = {{400, 300}}  // Center
};

// Minions during boss fight
stage.waves[1] = (EnemyWave){
    .spawnTime = 15.0f,  // After boss is damaged
    .enemyTypes = {/* Mixed support enemies */}
};

stage.hasSpecialEffect = true;  // Boss visual effects
stage.enemyHealthMultiplier = 2.0f;  // Boss is tanky
```

### Pattern 3: Chaos/Finale
```c
stage.waveCount = 5;  // Many waves
stage.maxEnemiesAlive = 8;  // Allow overlap

// Waves spawn quickly for chaotic feel
stage.waves[0].spawnTime = 0.0f;
stage.waves[1].spawnTime = 8.0f;
stage.waves[2].spawnTime = 15.0f;
stage.waves[3].spawnTime = 23.0f;
stage.waves[4].spawnTime = 32.0f;  // Final wave

// Mix all enemy types learned so far
```

## Troubleshooting

### Issue: Stage doesn't compile

**Check:**
1. Did you add the file to `Makefile`?
2. Does it include `"stage_common.h"`?
3. Is the function signature correct: `Stage CreateStageN(void)`?

### Issue: Enemies don't spawn

**Check:**
1. Is `waveCount` correct?
2. Are `spawnTime` values reasonable?
3. Is `enemyCount` > 0 for each wave?
4. Does `targetKills` match total enemies?

### Issue: Stage never completes

**Check:**
```c
// Count total enemies across all waves
int totalEnemies = 0;
for (int i = 0; i < stage.waveCount; i++) {
    totalEnemies += stage.waves[i].enemyCount;
}

// This should match:
stage.targetKills = totalEnemies;
```

**Note:** For SPLITTER enemies, target kills should be higher since they spawn additional enemies when damaged.

### Issue: Enemies spawn in wrong positions

**Check:**
- If `useRandomSpawn = false`, ensure `spawnPattern` has enough entries for `enemyCount`
- Screen coordinates: width = 800, height = 800 (approximately)
- Valid spawn range: x: 50-750, y: 50-750

### Issue: Stage feels unbalanced

**Playtest and Adjust:**
1. Record completion time
2. Note difficulty spikes
3. Adjust multipliers by ±0.1-0.3
4. Retest and iterate

## Example: Modifying Stage 1

Let's say Stage 1 is too easy. Here's how to balance:

**Current:**
```c
stage.enemyHealthMultiplier = 1.2f;
stage.enemySpeedMultiplier = 0.8f;
stage.waves[0].spawnDelay = 1.0f;
```

**Make it harder:**
```c
stage.enemyHealthMultiplier = 1.4f;  // +0.2
stage.enemySpeedMultiplier = 1.0f;   // +0.2
stage.waves[0].spawnDelay = 0.8f;    // -0.2 (faster spawns)
```

**Test:**
```bash
make clean && make test-stage-1
```

## Best Practices

1. **Document Everything**: Use the comment template for wave descriptions
2. **Incremental Testing**: Test after each wave addition
3. **Consistent Naming**: Follow existing stage names and themes
4. **Balance in Context**: Consider previous and next stages
5. **Player Perspective**: Think about what the player experiences
6. **Iterate**: First version is never perfect

## Resources

- `PLANNING.md`: Overall game architecture
- `STAGE_DESIGN.md`: Design philosophy for each stage
- `STAGE_WORKFLOW.md`: Git workflow for stage development
- `src/entities/managers/stages/stage_*.c`: Reference existing stages

## Quick Reference Card

```c
// Minimal viable stage
Stage CreateStageN(void) {
    Stage stage = {0};
    stage.stageNumber = N;
    strcpy(stage.name, "Stage Name");
    strcpy(stage.description, "Description");

    stage.waveCount = 2;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER,
                       ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };

    stage.targetKills = 8;
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 1.0f;
    stage.enemySpeedMultiplier = 1.0f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = RAYWHITE;
    stage.particleColor = DARKGRAY;

    return stage;
}
```

Happy stage building!
