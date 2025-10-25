/**
 * @file stage_9.c
 * @brief Stage 9: Chain Reaction
 *
 * CLUSTER enemies that create chain explosions.
 * One explosion can trigger another, creating cascading damage.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 9 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 3x CLUSTER + BASIC
 * - Wave 2 (8s): 4x mixed CLUSTER enemies
 * - Wave 3 (15s): 5x CLUSTER with variety
 * - Wave 4 (23s): 6x heavily mixed CLUSTER wave
 * - Wave 5 (32s): 4x CLUSTER in formation pattern
 *
 * Special mechanics:
 * - Chain explosions between cluster enemies
 * - 5 waves for maximum chaos
 * - 22 total kills required
 * - Formation spawn at final wave
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage9(void) {
    Stage stage = {0};
    stage.stageNumber = 9;
    strcpy(stage.name, "Chain Reaction");
    strcpy(stage.description, "Explosive enemies - One explosion triggers another!");

    stage.waveCount = 5;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_BASIC, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_ORBITER},
        .spawnDelay = 1.8f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 23.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[4] = (EnemyWave){
        .spawnTime = 32.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_CLUSTER},
        .spawnDelay = 0.5f,
        .useRandomSpawn = false,
        .spawnPattern = {{200, 200}, {600, 200}, {200, 600}, {600, 600}}
    };

    stage.targetKills = 22;
    stage.maxEnemiesAlive = 8;
    stage.enemyHealthMultiplier = 1.7f;
    stage.enemySpeedMultiplier = 1.4f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){255, 200, 200, 255};
    stage.particleColor = DARKPURPLE;

    return stage;
}
