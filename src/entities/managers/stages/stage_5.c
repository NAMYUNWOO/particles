/**
 * @file stage_5.c
 * @brief Stage 5: Orbital Mechanics
 *
 * ORBITER enemies with complex circular movement patterns.
 * Tests player's ability to track and predict enemy movements.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 5 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 3x ORBITER with fixed spawn pattern
 * - Wave 2 (8s): 4x mixed ORBITER, TRACKER, SPEEDY
 * - Wave 3 (16s): 5x mostly ORBITER and SPLITTER
 * - Wave 4 (25s): 3x ORBITER with formation pattern
 *
 * Special mechanics:
 * - Fixed spawn patterns for some waves
 * - Orbiters move in circular patterns
 * - 4 waves for increased complexity
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage5(void) {
    Stage stage = {0};
    stage.stageNumber = 5;
    strcpy(stage.name, "Orbital Mechanics");
    strcpy(stage.description, "Enemies with complex movement patterns");

    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 200}, {200, 400}, {600, 400}}
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 16.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 1.2f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 25.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 0.5f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 400}, {300, 400}, {500, 400}}
    };

    stage.targetKills = 15;
    stage.maxEnemiesAlive = 7;
    stage.enemyHealthMultiplier = 1.4f;
    stage.enemySpeedMultiplier = 1.2f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){220, 255, 220, 255};
    stage.particleColor = DARKGREEN;

    return stage;
}
