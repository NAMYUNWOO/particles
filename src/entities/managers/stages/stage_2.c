/**
 * @file stage_2.c
 * @brief Stage 2: The Hunt Begins
 *
 * Introduces TRACKER enemies that follow the player's position.
 * Teaches players to manage enemies with advanced movement patterns.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 2 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 2x TRACKER + BASIC
 * - Wave 2 (8s): 4x mixed TRACKER and BASIC enemies
 * - Wave 3 (18s): 3x TRACKER enemies
 *
 * Difficulty increase:
 * - Enemies have 1.2x health
 * - Enemies move 1.1x faster
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage2(void) {
    Stage stage = {0};
    stage.stageNumber = 2;
    strcpy(stage.name, "The Hunt Begins");
    strcpy(stage.description, "Beware! Enemies now track your position");

    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 18.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_TRACKER},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };

    stage.targetKills = 9;
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 1.2f;
    stage.enemySpeedMultiplier = 1.1f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){240, 240, 240, 255};
    stage.particleColor = DARKGRAY;

    return stage;
}
