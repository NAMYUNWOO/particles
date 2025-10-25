/**
 * @file stage_test.c
 * @brief Developer Test Mode Stage
 *
 * Clean environment for testing individual enemy types.
 * No waves, no win conditions, infinite time.
 */

#include "stage_common.h"
#include <string.h>

/**
 * Creates Test Mode Stage configuration
 *
 * Stage characteristics:
 * - No enemy waves (manual spawning only)
 * - No kill targets (infinite play)
 * - Clean visual environment
 * - Space grey background, white particles
 *
 * @return Stage struct with test configuration
 */
Stage CreateStageTest(void) {
    Stage stage = {0};
    stage.stageNumber = 0;  // Special stage number for test mode
    strcpy(stage.name, "Developer Test Mode");
    strcpy(stage.description, "Interactive enemy testing environment");

    stage.waveCount = 0;  // No automatic waves
    stage.targetKills = 0;  // No win condition
    stage.maxEnemiesAlive = 50;  // Higher limit for testing

    // Standard multipliers (no modifications)
    stage.enemyHealthMultiplier = 1.0f;
    stage.enemySpeedMultiplier = 1.0f;
    stage.enemySizeMultiplier = 1.0f;

    // Consistent visual theme
    stage.backgroundColor = (Color){52, 58, 64, 255};  // Space grey
    stage.particleColor = WHITE;  // White particles

    stage.hasSpecialEffect = false;
    stage.particleAttractionMultiplier = 1.0f;

    return stage;
}
