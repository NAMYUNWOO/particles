/**
 * @file stage_4.c
 * @brief Stage 4: Divide and Conquer
 *
 * SPLITTER enemies that split into smaller enemies when destroyed.
 * Requires strategic thinking to manage enemy count.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 4 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 2x SPLITTER + BASIC
 * - Wave 2 (10s): 3x SPLITTER enemies
 * - Wave 3 (20s): 4x mixed SPLITTER and SPEEDY
 *
 * Special mechanics:
 * - Splitters create additional enemies when destroyed
 * - Higher target kills (20) due to splitting
 * - Max 8 enemies alive simultaneously
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage4(void) {
    Stage stage = {0};
    stage.stageNumber = 4;
    strcpy(stage.name, "Divide and Conquer");
    strcpy(stage.description, "Enemies split when destroyed - Control the chaos!");

    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 10.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_TRACKER},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 20.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_SPLITTER},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };

    stage.targetKills = 20;  // More kills needed due to splitting
    stage.maxEnemiesAlive = 8;
    stage.enemyHealthMultiplier = 1.3f;
    stage.enemySpeedMultiplier = 1.0f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){255, 220, 220, 255};
    stage.particleColor = MAROON;

    return stage;
}
