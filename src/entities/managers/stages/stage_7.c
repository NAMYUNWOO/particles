/**
 * @file stage_7.c
 * @brief Stage 7: Quantum Flux
 *
 * TELEPORTER enemies that can teleport across the screen.
 * Requires constant awareness and quick reactions.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 7 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 2x TELEPORTER + BASIC
 * - Wave 2 (6s): 4x mixed TELEPORTER and others
 * - Wave 3 (14s): 5x TELEPORTER with various enemies
 * - Wave 4 (22s): 3x TELEPORTER enemies
 *
 * Special mechanics:
 * - Enemies can teleport unpredictably
 * - 1.5x health, 1.3x speed
 * - 14 total kills required
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage7(void) {
    Stage stage = {0};
    stage.stageNumber = 7;
    strcpy(stage.name, "Quantum Flux");
    strcpy(stage.description, "Enemies can teleport - Stay alert!");

    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 6.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TRACKER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 14.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_ORBITER, ENEMY_TYPE_TELEPORTER},
        .spawnDelay = 1.2f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 22.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_TELEPORTER},
        .spawnDelay = 0.8f,
        .useRandomSpawn = true
    };

    stage.targetKills = 14;
    stage.maxEnemiesAlive = 6;
    stage.enemyHealthMultiplier = 1.5f;
    stage.enemySpeedMultiplier = 1.3f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey background
    stage.particleColor = WHITE;  // White particles for maximum visibility

    return stage;
}
