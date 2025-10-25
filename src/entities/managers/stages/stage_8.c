/**
 * @file stage_8.c
 * @brief Stage 8: Magnetic Storm
 *
 * REPULSOR enemies that repel particles, making them harder to hit.
 * Forces players to find new strategies for combat.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 8 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 2x REPULSOR + BASIC
 * - Wave 2 (8s): 3x REPULSOR and TRACKER
 * - Wave 3 (16s): 5x mixed REPULSOR enemies
 * - Wave 4 (25s): 4x REPULSOR and SPLITTER
 *
 * Special mechanics:
 * - Repulsors push particles away
 * - 0.7x particle attraction (harder to control)
 * - 1.6x health, challenging enemies
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage8(void) {
    Stage stage = {0};
    stage.stageNumber = 8;
    strcpy(stage.name, "Magnetic Storm");
    strcpy(stage.description, "Enemies repel particles - Find new strategies!");

    stage.waveCount = 4;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 2,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_BASIC},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_TRACKER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 2.5f,
        .useRandomSpawn = true
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 16.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    stage.waves[3] = (EnemyWave){
        .spawnTime = 25.0f,
        .enemyCount = 4,
        .enemyTypes = {ENEMY_TYPE_REPULSOR, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_SPLITTER, ENEMY_TYPE_REPULSOR},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };

    stage.targetKills = 14;
    stage.maxEnemiesAlive = 7;
    stage.enemyHealthMultiplier = 1.6f;
    stage.enemySpeedMultiplier = 1.1f;
    stage.enemySizeMultiplier = 1.0f;
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey background
    stage.particleColor = WHITE;  // White particles for maximum visibility
    stage.particleAttractionMultiplier = 0.7f;  // Particles are harder to control

    return stage;
}
