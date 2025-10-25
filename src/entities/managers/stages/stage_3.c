/**
 * @file stage_3.c
 * @brief Stage 3: Speed Demons
 *
 * Fast SPEEDY enemies with zigzag movement patterns.
 * Challenges player's reflexes and boost management.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 3 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 3x mixed SPEEDY and BASIC
 * - Wave 2 (7s): 5x mostly SPEEDY enemies
 * - Wave 3 (15s): 4x SPEEDY enemies
 *
 * Difficulty increase:
 * - Enemies move 1.5x faster
 * - More enemies (12 total kills required)
 * - Lower enemy health (1.0x) for speedies
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage3(void) {
    Stage stage = {0};
    stage.stageNumber = 3;
    strcpy(stage.name, "Speed Demons");
    strcpy(stage.description, "Fast enemies incoming! Use boost wisely");

    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 7.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_TRACKER, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 0.8f,
        .useRandomSpawn = true
    };

    stage.targetKills = 12;
    stage.maxEnemiesAlive = 6;
    stage.enemyHealthMultiplier = 1.0f;  // Speedy enemies have less health
    stage.enemySpeedMultiplier = 1.5f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey background
    stage.particleColor = WHITE;  // White particles for maximum visibility

    return stage;
}
