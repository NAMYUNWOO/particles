/**
 * @file stage_6.c
 * @brief Stage 6: Guardian of the Void (BOSS BATTLE)
 *
 * First boss encounter with BOSS_1 enemy type.
 * Features minion waves during the battle.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Stage 6 configuration
 *
 * Enemy composition:
 * - Wave 1 (2s): 1x BOSS_1 (center spawn)
 * - Wave 2 (10s): 6x minions (mixed types)
 *
 * Boss mechanics:
 * - 2.0x health multiplier
 * - 1.5x size (larger than normal enemies)
 * - Special effect enabled
 * - Boss has shields and phase changes
 *
 * @return Stage struct with all configuration
 */
Stage CreateStage6(void) {
    Stage stage = {0};
    stage.stageNumber = 6;
    strcpy(stage.name, "Guardian of the Void");
    strcpy(stage.description, "BOSS BATTLE - Defeat the Guardian!");

    stage.waveCount = 2;
    // Boss spawn
    stage.waves[0] = (EnemyWave){
        .spawnTime = 2.0f,
        .enemyCount = 1,
        .enemyTypes = {ENEMY_TYPE_BOSS_1},
        .spawnDelay = 0.0f,
        .useRandomSpawn = false,
        .spawnPattern = {{400, 300}}
    };
    // Minions during boss fight
    stage.waves[1] = (EnemyWave){
        .spawnTime = 10.0f,
        .enemyCount = 6,
        .enemyTypes = {ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER, ENEMY_TYPE_BASIC, ENEMY_TYPE_SPEEDY, ENEMY_TYPE_BASIC, ENEMY_TYPE_TRACKER},
        .spawnDelay = 3.0f,
        .useRandomSpawn = true
    };

    stage.targetKills = 7;  // Boss + minions
    stage.maxEnemiesAlive = 5;
    stage.enemyHealthMultiplier = 2.0f;  // Boss has much more health
    stage.enemySpeedMultiplier = 0.8f;
    stage.enemySizeMultiplier = 1.5f;  // Boss is larger
    stage.backgroundColor = (Color){40, 40, 40, 255};
    stage.particleColor = RED;
    stage.hasSpecialEffect = true;

    return stage;
}
