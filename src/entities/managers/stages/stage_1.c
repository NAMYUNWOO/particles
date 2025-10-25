/**
 * @file stage_1.c
 * @brief Stage 1: Gravitational Anomaly
 *
 * Introduces the Blackhole enemy with basic enemies.
 * Teaches players to use gravity against enemies and shows
 * that blackhole awakens when alone.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 1 configuration
 *
 * Enemy composition:
 * - Wave 1 (0s): 5x BASIC enemies
 * - Wave 2 (8s): 1x BLACKHOLE enemy (center-top position)
 * - Wave 3 (15s): 3x mixed (BASIC, TRACKER, BASIC)
 *
 * Special mechanics:
 * - Blackhole is invulnerable while other enemies exist
 * - Blackhole transforms after all enemies die
 * - Stronger particle attraction (1.5x)
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage1(void) {
    Stage stage = {0};
    stage.stageNumber = 1;
    strcpy(stage.name, "Gravitational Anomaly");
    strcpy(stage.description, "Use gravity against enemies - Blackhole awakens when alone!");

    // Introduce blackhole enemy with some basic enemies
    stage.waveCount = 3;
    stage.waves[0] = (EnemyWave){
        .spawnTime = 0.0f,
        .enemyCount = 5,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC, ENEMY_TYPE_BASIC},
        .spawnDelay = 1.0f,
        .useRandomSpawn = true
    };
    stage.waves[1] = (EnemyWave){
        .spawnTime = 8.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BLACKHOLE},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 200}}  // Center-top position
    };
    stage.waves[2] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 3,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };

    stage.targetKills = 9;
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 1.2f;  // Blackholes are slightly tougher
    stage.enemySpeedMultiplier = 0.8f;   // Normal speed (blackhole has its own speed boost)
    stage.enemySizeMultiplier = 1.3f;    // Normal size (blackhole already has custom size)
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey background
    stage.particleColor = WHITE;  // White particles for maximum visibility
    stage.particleAttractionMultiplier = 1.5f;  // Stronger particle attraction

    return stage;
}
