/**
 * @file stage_10.c
 * @brief Stage 10: The Particle Overlord (FINAL BOSS)
 *
 * Epic final boss battle with BOSS_FINAL enemy type.
 * Multiple minion waves and phase transitions.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 10 configuration
 *
 * Enemy composition:
 * - Wave 1 (3s): 1x BOSS_FINAL (center spawn)
 * - Wave 2 (15s): 8x minions (all enemy types)
 * - Wave 3 (35s): 6x second minion wave
 *
 * Boss mechanics:
 * - 3.0x health multiplier (massive health pool)
 * - 2.0x size (much larger than other enemies)
 * - Special effect enabled
 * - Boss has all abilities from previous bosses
 * - 15 total kills (boss + minions)
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage10(void) {
    Stage stage = {0};
    stage.stageNumber = 10;
    strcpy(stage.name, "The Particle Overlord");
    strcpy(stage.description, "FINAL BOSS - The ultimate challenge awaits!");

    stage.waveCount = 3;
    // Final boss
    stage.waves[0] = (EnemyWave){
        .spawnTime = 3.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BOSS_FINAL},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 400}}
    };
    // First minion wave
    stage.waves[1] = (EnemyWave){
        .spawnTime = 15.0f,
        .enemyCount = 8,
        .enemyTypes = {ENEMY_TYPE_SPEEDY, ENEMY_TYPE_TRACKER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPLITTER,
                      ENEMY_TYPE_ORBITER, ENEMY_TYPE_REPULSOR, ENEMY_TYPE_CLUSTER, ENEMY_TYPE_BASIC},
        .spawnDelay = 2.0f,
        .useRandomSpawn = true
    };
    // Second minion wave (during boss phase 2)
    stage.waves[2] = (EnemyWave){
        .spawnTime = 35.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_REPULSOR,
                      ENEMY_TYPE_CLUSTER, ENEMY_TYPE_TELEPORTER, ENEMY_TYPE_SPEEDY},
        .spawnDelay = 1.5f,
        .useRandomSpawn = true
    };

    stage.targetKills = 15;  // Boss + all minions
    stage.maxEnemiesAlive = 10;
    stage.enemyHealthMultiplier = 3.0f;  // Final boss has massive health
    stage.enemySpeedMultiplier = 1.5f;
    stage.enemySizeMultiplier = 2.0f;  // Final boss is much larger
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey background
    stage.particleColor = WHITE;  // White particles for maximum visibility
    stage.hasSpecialEffect = true;

    return stage;
}
